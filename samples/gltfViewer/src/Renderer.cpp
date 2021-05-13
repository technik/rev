//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2021 Carmelo J Fdez-Aguera
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#include "Renderer.h"

#include <core/platform/osHandler.h>

#include <graphics/backend/Vulkan/gpuBuffer.h>
#include <graphics/scene/Material.h>
#include <graphics/Image.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_vulkan.h>
#include <imgui/backends/imgui_impl_win32.h>

// Imgui windows handler
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace rev
{
	//---------------------------------------------------------------------------------------------------------------------
	size_t Renderer::init(
		gfx::RenderContextVulkan& ctxt,
		const math::Vec2u& windowSize,
		const SceneDesc& scene)
	{
		m_windowSize = windowSize;

		m_ctxt = &ctxt;

		m_frameBuffers = std::make_unique<gfx::FrameBufferManager>(ctxt.device());

		auto device = ctxt.device();

		// Create semaphores
		m_imageAvailableSemaphore = device.createSemaphore({});

		const size_t maxSceneTextures = scene.m_textures.size();

		createRenderTargets();
		createRenderPasses();
		createDescriptorLayouts(maxSceneTextures);
		createShaderPipelines();
		loadIBLLUT();

		// Allocate matrix buffers
		auto& alloc = m_ctxt->allocator();
		m_mtxBuffers.resize(2);
		const size_t maxNumInstances = scene.m_sceneInstances.numInstances();
		const size_t maxNumMaterials = scene.m_materials.size();

		m_mtxBuffers[0] = alloc.createBufferForMapping(sizeof(math::Mat44f) * maxNumInstances, vk::BufferUsageFlagBits::eStorageBuffer, m_ctxt->graphicsQueueFamily());
		m_mtxBuffers[1] = alloc.createBufferForMapping(sizeof(math::Mat44f) * maxNumInstances, vk::BufferUsageFlagBits::eStorageBuffer, m_ctxt->graphicsQueueFamily());

		// Allocate materials buffer
		size_t streamToken = 0;
		if (!scene.m_materials.empty())
		{
			m_materialsBuffer = alloc.createGpuBuffer(
				sizeof(gfx::PBRMaterial) * maxNumMaterials,
				vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst,
				m_ctxt->graphicsQueueFamily());
			streamToken = alloc.asyncTransfer(*m_materialsBuffer, scene.m_materials.data(), scene.m_materials.size());
		}

		// Init descriptor sets
		createDescriptorSets(maxSceneTextures);
		// Update descriptor sets
		fillConstantDescriptorSets(scene);

		initImGui();

		return streamToken;
	}

	//---------------------------------------------------------------------------------------------------------------------
	void Renderer::end()
	{
		destroyFrameBuffers();
		destroyRenderTargets();

		auto device = m_ctxt->device();
		m_gBufferPipeline.reset();
		device.destroyPipelineLayout(m_gbufferPipelineLayout);
		device.destroyRenderPass(m_uiRenderPass->vkPass());
		device.destroySemaphore(m_imageAvailableSemaphore);
	}

	//---------------------------------------------------------------------------------------------------------------------
	void Renderer::onResize(const math::Vec2u& windowSize)
	{
		m_windowSize = windowSize;
		if (windowSize.x() == 0 || windowSize.y() == 0)
			return; // Avoid recreating an empty swap chain

		m_ctxt->resizeSwapchain(windowSize);
		createRenderTargets();
		
		// Update render passes
		m_uiRenderPass->setDepthTarget(*m_gBufferZ);

		// Invalidate frame buffers
		m_frameBuffers->clear();
	}

	//---------------------------------------------------------------------------------------------------------------------
	void Renderer::render(SceneDesc& scene, bool geometryReady)
	{
		if (m_windowSize.x() == 0 || m_windowSize.y() == 0)
			return; // Don't try to render while minimized

		// Watch for shader reload
		m_shaderWatcher->update();

		// Update frame state
		m_frameConstants.lightColor = scene.lightColor;
		m_frameConstants.ambientColor = scene.ambientColor;
		m_frameConstants.lightDir = normalize(scene.lightDir);
		float aspect = float(m_windowSize.x()) / m_windowSize.y();
		m_frameConstants.proj = scene.proj;
		m_frameConstants.view = scene.view;

		// Record frame
		auto cmd = m_ctxt->getNewRenderCmdBuffer();
		cmd.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

		// Render the Geometry/UI pass
		m_uiRenderPass->setColorTargets({ &m_ctxt->swapchainAquireNextImage(m_imageAvailableSemaphore, cmd) });
		math::Vec3f ambient = m_frameConstants.ambientColor;
		ambient.x() = ambient.x() / (1 + ambient.x());
		ambient.y() = ambient.y() / (1 + ambient.y());
		ambient.z() = ambient.z() / (1 + ambient.z());
		m_uiRenderPass->setClearColor(ambient);
		m_uiRenderPass->begin(cmd, m_windowSize);

		// Render a triangle if the scene is loaded
		if (geometryReady)
		{
			// Update per instance model matrices
			auto mtxDst = m_ctxt->allocator().mapBuffer<math::Mat44f>(*m_mtxBuffers[m_doubleBufferNdx]);
			scene.m_sceneInstances.updatePoses(mtxDst);
			m_ctxt->allocator().unmapBuffer(mtxDst);

			// Bind pipeline
			m_gBufferPipeline->bind(cmd);

			// Update descriptor set with this frame's matrices
			cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_gbufferPipelineLayout, 0, m_frameDescs[m_doubleBufferNdx], {});

			cmd.pushConstants<FramePushConstants>(
				m_gbufferPipelineLayout,
				vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
				0,
				m_frameConstants);

			// Draw all instances in a single batch
			m_uiRenderPass->drawGeometry(cmd, scene.m_sceneInstances, scene.m_rasterData);

			m_doubleBufferNdx ^= 1;
		}

		// Finish ImGui
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

		m_uiRenderPass->end(cmd);

		cmd.end();

		vk::PipelineStageFlags waitFlags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		vk::SubmitInfo submitInfo(
			1, &m_imageAvailableSemaphore, &waitFlags, // wait
			1, &cmd, // commands
			1, &m_ctxt->readyToPresentSemaphore()); // signal
		m_ctxt->graphicsQueue().submit(submitInfo);
		m_ctxt->swapchainPresent();
	}

	//---------------------------------------------------------------------------------------------------------------------
	constexpr uint32_t RF_OVERRIDE_MATERIAL = 1<<0;
	constexpr uint32_t RF_ENV_PROBE = 1 << 1;
	constexpr uint32_t RF_DISABLE_AO = 1 << 2;
	constexpr uint32_t RF_NO_NORMAL_MAP = 1<<3;

	void Renderer::updateUI()
	{
		if (ImGui::CollapsingHeader("Renderer"))
		{
			// Render flags
			bool overrideMaterial = renderFlag(RF_OVERRIDE_MATERIAL);
			ImGui::Checkbox("Override Material", &overrideMaterial);
			bool useEnvProbe = renderFlag(RF_ENV_PROBE);
			ImGui::Checkbox("Env. Probe", &useEnvProbe);
			bool enableAO = !renderFlag(RF_DISABLE_AO);
			ImGui::Checkbox("Enable AO", &enableAO);
			bool enableNormalMaps = !renderFlag(RF_NO_NORMAL_MAP);
			ImGui::Checkbox("Normal Maps", &enableNormalMaps);

			m_frameConstants.renderFlags =
				(overrideMaterial ? RF_OVERRIDE_MATERIAL : 0) |
				(useEnvProbe ? RF_ENV_PROBE : 0) |
				(!enableAO ? RF_DISABLE_AO : 0) |
				(!enableNormalMaps ? RF_NO_NORMAL_MAP : 0);

			if (overrideMaterial)
			{
				ImGui::ColorPicker3("Base Color", m_frameConstants.overrideBaseColor.data());
				ImGui::SliderFloat("Metallic", &m_frameConstants.overrideMetallic, 0.f, 1.f);
				ImGui::SliderFloat("Roughness", &m_frameConstants.overrideRoughness, 0.f, 1.f);
				ImGui::SliderFloat("Clear Coat", &m_frameConstants.overrideClearCoat, 0.f, 1.f);
			}
		}
	}

	//---------------------------------------------------------------------------------------------------------------------
	void Renderer::createDescriptorLayouts(size_t numTextures)
	{
		auto device = m_ctxt->device();

		// Matrices
		vk::DescriptorSetLayoutBinding matricesBinding(0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eVertex);
		//Materials
		vk::DescriptorSetLayoutBinding materialsBinding(1, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);
		// IBL LUT
		vk::DescriptorSetLayoutBinding iblBinding(2, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment);

		// Textures
		vk::DescriptorSetLayoutBinding textureBinding(3, vk::DescriptorType::eCombinedImageSampler, (uint32_t)numTextures, vk::ShaderStageFlagBits::eFragment);

		// Create layout
		std::vector<vk::DescriptorSetLayoutBinding> bindings{ matricesBinding, materialsBinding, iblBinding, textureBinding };
		vk::DescriptorSetLayoutCreateInfo setLayoutInfo({}, bindings);
		m_frameDescLayout = device.createDescriptorSetLayout(setLayoutInfo);
	}

	//---------------------------------------------------------------------------------------------------------------------
	void Renderer::createDescriptorSets(size_t numTextures)
	{
		auto device = m_ctxt->device();

		const uint32_t numDescriptorsPerFrame = 3 + numTextures;
		const uint32_t numDescriptorBindings = numDescriptorsPerFrame * 2; // 3 per frame * 2 frames
		vk::DescriptorPoolSize poolSize[2] = {
			{vk::DescriptorType::eStorageBuffer, 2 * 2},
			{vk::DescriptorType::eCombinedImageSampler, (1+(uint32_t)numTextures)*2}
		};

		auto poolInfo = vk::DescriptorPoolCreateInfo({}, numDescriptorBindings);
		poolInfo.pPoolSizes = poolSize;
		poolInfo.poolSizeCount = 2;
		m_descPool = device.createDescriptorPool(poolInfo);

		vk::DescriptorSetLayout setLayouts[2] = { m_frameDescLayout, m_frameDescLayout };
		vk::DescriptorSetAllocateInfo setInfo(m_descPool, 2, setLayouts);
		m_frameDescs = device.allocateDescriptorSets(setInfo);
	}
	//---------------------------------------------------------------------------------------------------------------------
	void Renderer::fillConstantDescriptorSets(const SceneDesc& scene)
	{
		auto device = m_ctxt->device();

		for (int frameNdx = 0; frameNdx < 2; ++frameNdx)
		{
			vk::WriteDescriptorSet writeInfo;
			writeInfo.dstSet = m_frameDescs[frameNdx];
			writeInfo.dstArrayElement = 0;

			// Matrix buffer
			vk::DescriptorBufferInfo writeBufferInfo(m_mtxBuffers[frameNdx]->buffer(), 0, m_mtxBuffers[frameNdx]->size());
			writeInfo.dstBinding = 0;
			writeInfo.descriptorType = vk::DescriptorType::eStorageBuffer;
			writeInfo.descriptorCount = 1;
			writeInfo.pBufferInfo = &writeBufferInfo;
			device.updateDescriptorSets(writeInfo, {});
			
			// Materials buffer
			vk::DescriptorBufferInfo materialsBufferInfo(m_materialsBuffer->buffer(), 0, m_materialsBuffer->size());
			writeInfo.dstBinding = 1;
			writeInfo.pBufferInfo = &materialsBufferInfo;
			device.updateDescriptorSets(writeInfo, {});

			// IBL texture
			vk::DescriptorImageInfo iblInfo;
			iblInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			iblInfo.imageView = m_iblLUT->image->view();
			iblInfo.sampler = m_iblLUT->sampler;
			writeInfo.descriptorType = vk::DescriptorType::eCombinedImageSampler;
			writeInfo.dstBinding = 2;
			writeInfo.pImageInfo = &iblInfo;
			device.updateDescriptorSets(writeInfo, {});

			// Material textures
			if (scene.m_textures.size() > 0)
			{
				std::vector<vk::DescriptorImageInfo> texInfo(scene.m_textures.size());
				for(size_t i = 0; i < scene.m_textures.size(); ++i)
				{
					auto& texture = *scene.m_textures[i];
					texInfo[i].imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
					texInfo[i].imageView = texture.image->view();
					texInfo[i].sampler = texture.sampler;
				}
				writeInfo.descriptorType = vk::DescriptorType::eCombinedImageSampler;
				writeInfo.dstBinding = 3;
				writeInfo.descriptorCount = scene.m_textures.size();
				writeInfo.pImageInfo = texInfo.data();
				device.updateDescriptorSets(writeInfo, {});
			}
		}
	}

	//---------------------------------------------------------------------------------------------------------------------
	void Renderer::createRenderPasses()
	{
		// UI Render pass
		m_uiRenderPass = std::make_unique<RenderPass>(
			m_ctxt->createRenderPass({ m_ctxt->swapchainFormat(), vk::Format::eD32Sfloat }),
			*m_frameBuffers);
		m_uiRenderPass->setDepthTarget(*m_gBufferZ);
		m_uiRenderPass->setClearDepth(0.f);
	}

	//---------------------------------------------------------------------------------------------------------------------
	void Renderer::createShaderPipelines()
	{
		auto device = m_ctxt->device();

		m_shaderWatcher = std::make_unique<core::FolderWatcher>(core::FolderWatcher::path("../shaders"));

		// G-Buffer pipeline
		vk::PushConstantRange camerasPushRange(
			vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
			0, sizeof(FramePushConstants));

		vk::PipelineLayoutCreateInfo layoutInfo({},
			1, &m_frameDescLayout, // Descriptor sets
			1, &camerasPushRange); // Push constants

		m_gbufferPipelineLayout = device.createPipelineLayout(layoutInfo);
		m_gBufferPipeline = std::make_unique<gfx::RasterPipeline>(
			device,
			m_gbufferPipelineLayout,
			m_uiRenderPass->vkPass(),
			"../shaders/gbuffer.vert.spv",
			"../shaders/gbuffer.frag.spv",
			true);
		m_shaderWatcher->listen([this](auto paths) { m_gBufferPipeline->invalidate(); });
	}

	//---------------------------------------------------------------------------------------------------------------------
	void Renderer::createRenderTargets()
	{
		auto windowSize = m_ctxt->windowSize();
		auto& alloc = m_ctxt->allocator();

		m_gBufferNormals = alloc.createImageBuffer("normals", windowSize, vk::Format::eR32G32B32A32Sfloat, vk::ImageUsageFlagBits::eColorAttachment, m_ctxt->graphicsQueueFamily());
		m_gBufferPBR = alloc.createImageBuffer("PBR", windowSize, vk::Format::eR32G32B32A32Sfloat, vk::ImageUsageFlagBits::eColorAttachment, m_ctxt->graphicsQueueFamily());
		m_gBufferZ = alloc.createDepthBuffer(
			"Depth",
			windowSize,
			vk::Format::eD32Sfloat,
			vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferDst,
			m_ctxt->graphicsQueueFamily());

		// Transition new images to general layout
		m_ctxt->transitionImageLayout(m_gBufferZ->image(), m_gBufferZ->format(), vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral, true);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Renderer::destroyRenderTargets()
	{
		m_gBufferNormals = nullptr;
		m_gBufferPBR = nullptr;
		m_gBufferZ = nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Renderer::destroyFrameBuffers()
	{
		m_frameBuffers->clear();
	}

	void Renderer::loadIBLLUT()
	{
		auto image = gfx::Image::load("shaders/ibl_brdf.hdr", 4, false);
		m_iblLUT = m_ctxt->allocator().createTexture(
			*m_ctxt,
			"IBL LUT",
			image->size(),
			vk::Format::eR32G32B32A32Sfloat,
			vk::SamplerAddressMode::eClampToEdge,
			vk::SamplerAddressMode::eClampToEdge,
			false,
			0,
			image->data(),
			vk::ImageUsageFlagBits::eSampled,
			m_ctxt->graphicsQueueFamily()
		);
	}

	//------------------------------------------------------------------------------------------------
	void Renderer::initImGui()
	{
		ImGui::CreateContext();

		auto& rc = *m_ctxt;
		auto nativeWindow = rc.nativeWindow();
		ImGui_ImplWin32_Init(&nativeWindow);

		vk::DescriptorPoolSize pool_sizes[] =
		{
			{ vk::DescriptorType::eSampler, 1000 },
			{ vk::DescriptorType::eCombinedImageSampler, 1000 },
			{ vk::DescriptorType::eSampledImage, 1000 },
			{ vk::DescriptorType::eStorageImage, 1000 },
			{ vk::DescriptorType::eUniformTexelBuffer, 1000 },
			{ vk::DescriptorType::eStorageTexelBuffer, 1000 },
			{ vk::DescriptorType::eUniformBuffer, 1000 },
			{ vk::DescriptorType::eStorageBuffer, 1000 },
			{ vk::DescriptorType::eUniformBufferDynamic, 1000 },
			{ vk::DescriptorType::eStorageBufferDynamic, 1000 },
			{ vk::DescriptorType::eInputAttachment, 1000 }
		};

		vk::DescriptorPoolCreateInfo pool_info = {};
		pool_info.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;

		ImGui_ImplVulkan_InitInfo initInfo{};
		initInfo.Instance = rc.instance();
		initInfo.Device = rc.device();
		initInfo.PhysicalDevice = rc.physicalDevice();
		initInfo.Queue = rc.graphicsQueue();
		initInfo.QueueFamily = rc.graphicsQueueFamily();
		initInfo.Subpass = 0;
		initInfo.ImageCount = 2;
		initInfo.MinImageCount = 2;
		initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		initInfo.DescriptorPool = rc.device().createDescriptorPool(pool_info);

		ImGui_ImplVulkan_Init(&initInfo, m_uiRenderPass->vkPass());

		auto& os = *core::OSHandler::get();
		os += [=](MSG msg) {
			// ImGui handler
			ImGui_ImplWin32_WndProcHandler(nativeWindow, msg.message, msg.wParam, msg.lParam);
			return false;
		};

		// Init font
		auto cmd = m_ctxt->getNewRenderCmdBuffer();
		cmd.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
		ImGui_ImplVulkan_CreateFontsTexture(cmd);

		cmd.end();

		vk::SubmitInfo submitInfo(
			0, nullptr, nullptr, // wait
			1, &cmd, // commands
			0, nullptr); // signal
		m_ctxt->graphicsQueue().submit(submitInfo);
	}
}