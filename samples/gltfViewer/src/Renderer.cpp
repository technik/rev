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

		// Full screen vertices
		auto& alloc = m_ctxt->allocator();
		m_fullScreenIndices = alloc.createGpuBuffer(
			3 * sizeof(uint32_t),
			vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
			ctxt.graphicsQueueFamily());
		uint32_t indices[3] = { 0, 1, 2 };
		size_t streamToken = alloc.asyncTransfer(*m_fullScreenIndices, indices, 3);

		// Allocate matrix buffers
		m_mtxBuffers.resize(2);
		const size_t maxNumInstances = scene.m_sceneInstances.numInstances();
		const size_t maxNumMaterials = scene.m_materials.size();

		m_mtxBuffers[0] = alloc.createBufferForMapping(sizeof(math::Mat44f) * maxNumInstances, vk::BufferUsageFlagBits::eStorageBuffer, m_ctxt->graphicsQueueFamily());
		m_mtxBuffers[1] = alloc.createBufferForMapping(sizeof(math::Mat44f) * maxNumInstances, vk::BufferUsageFlagBits::eStorageBuffer, m_ctxt->graphicsQueueFamily());

		// Allocate materials buffer
		if (!scene.m_materials.empty())
		{
			m_materialsBuffer = alloc.createGpuBuffer(
				sizeof(gfx::PBRMaterial) * maxNumMaterials,
				vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst,
				m_ctxt->graphicsQueueFamily());
			streamToken = alloc.asyncTransfer(*m_materialsBuffer, scene.m_materials.data(), scene.m_materials.size());
		}

		// Update descriptor sets
		fillConstantDescriptorSets(scene);

		gfx::DescriptorSetUpdate renderBufferUpdates(m_postProDescriptorSets, 0);
		renderBufferUpdates.addImage("HDR Light", m_hdrLightBuffer);
		renderBufferUpdates.send();

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
		device.destroyPipelineLayout(m_postProcessPipelineLayout);
		device.destroyRenderPass(m_uiRenderPass->vkPass());
		device.destroyRenderPass(m_hdrLightPass->vkPass());
		device.destroySemaphore(m_imageAvailableSemaphore);
	}

	//---------------------------------------------------------------------------------------------------------------------
	void Renderer::onResize(const math::Vec2u& intendedSize)
	{
		if (intendedSize.x() == 0 || intendedSize.y() == 0)
			return; // Avoid recreating an empty swap chain

		m_windowSize = m_ctxt->resizeSwapchain(intendedSize);
		createRenderTargets();
		
		// Update render passes
		m_hdrLightPass->setDepthTarget(*m_zBuffer);
		m_hdrLightPass->setColorTargets({ m_hdrLightBuffer.get() });

		gfx::DescriptorSetUpdate renderBufferUpdates(m_postProDescriptorSets, 0);
		renderBufferUpdates.addImage("HDR Light", m_hdrLightBuffer);
		renderBufferUpdates.send();

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

		// Render passes
		renderGeometryPass(scene, geometryReady);
		renderPostProPass();

		// Swapchain update
		m_ctxt->swapchainPresent();
	}

	//---------------------------------------------------------------------------------------------------------------------
	void Renderer::renderGeometryPass(SceneDesc& scene, bool geometryReady)
	{

		// Update frame state
		m_frameConstants.lightColor = scene.lightColor;
		m_frameConstants.ambientColor = scene.ambientColor;
		m_frameConstants.lightDir = normalize(scene.lightDir);
		float aspect = float(m_windowSize.x()) / m_windowSize.y();
		m_frameConstants.proj = scene.proj;
		m_frameConstants.view = scene.view;

		// Render geometry if the scene is loaded
		auto scope = m_ctxt->getScopedCmdBuffer(m_ctxt->graphicsQueue());
		auto cmd = scope.cmd;
		m_hdrLightPass->begin(cmd, m_windowSize);

		if (geometryReady)
		{
			// Update per instance model matrices
			auto mtxDst = m_ctxt->allocator().mapBuffer<math::Mat44f>(*m_mtxBuffers[m_doubleBufferNdx]);
			scene.m_sceneInstances.updatePoses(mtxDst);
			m_ctxt->allocator().unmapBuffer(mtxDst);

			// Bind pipeline
			m_gBufferPipeline->bind(cmd);

			// Update descriptor set with this frame's matrices
			cmd.bindDescriptorSets(
				vk::PipelineBindPoint::eGraphics,
				m_gbufferPipelineLayout,
				0, m_geometryDescriptorSets.getDescriptor(m_doubleBufferNdx), {});

			cmd.pushConstants<FramePushConstants>(
				m_gbufferPipelineLayout,
				vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
				0,
				m_frameConstants);

			// Draw all instances in a single batch
			m_uiRenderPass->drawGeometry(cmd, scene.m_sceneInstances, scene.m_rasterData);

			m_doubleBufferNdx ^= 1;
		}

		m_hdrLightPass->end(cmd);
	}

	//---------------------------------------------------------------------------------------------------------------------
	void Renderer::renderPostProPass()
	{
		auto cmd = m_ctxt->getNewRenderCmdBuffer();
		cmd.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

		m_ctxt->allocator().transitionImageLayout(cmd, m_hdrLightBuffer->image(), m_hdrLightBuffer->format(), vk::ImageLayout::eGeneral, vk::ImageLayout::eShaderReadOnlyOptimal, false);

		m_uiRenderPass->setColorTargets({ &m_ctxt->swapchainAquireNextImage(m_imageAvailableSemaphore, cmd) });
		m_uiRenderPass->setClearColor(m_frameConstants.ambientColor);
		m_uiRenderPass->begin(cmd, m_windowSize);

		m_postProConstants.windowSize = math::Vec2f(m_windowSize.x(), m_windowSize.y());
		m_postProConstants.ambientColor = m_frameConstants.ambientColor;
		m_postProConstants.renderFlags = {};
		m_postProConstants.bloom = 0.f;

		// Post-process HDR image
		m_postProPipeline->bind(cmd);
		
		cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
			m_postProcessPipelineLayout,
			0, m_postProDescriptorSets.getDescriptor(0), {});

		cmd.pushConstants<PostProPushConstants>(
			m_postProcessPipelineLayout,
			vk::ShaderStageFlagBits::eFragment,
			0,
			m_postProConstants);

		cmd.bindIndexBuffer(m_fullScreenIndices->buffer(), {}, vk::IndexType::eUint32);

		cmd.drawIndexed(3, 1, 0, 0, 0);


		// Finish ImGui
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

		m_uiRenderPass->end(cmd);

		m_ctxt->allocator().transitionImageLayout(cmd, m_hdrLightBuffer->image(), m_hdrLightBuffer->format(), vk::ImageLayout::eShaderReadOnlyOptimal, vk::ImageLayout::eGeneral, false);

		cmd.end();

		vk::PipelineStageFlags waitFlags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		vk::SubmitInfo submitInfo(
			1, &m_imageAvailableSemaphore, &waitFlags, // wait
			1, &cmd, // commands
			1, &m_ctxt->readyToPresentSemaphore()); // signal
		m_ctxt->graphicsQueue().submit(submitInfo);
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

			float fStops = log2f(m_postProConstants.exposure);
			ImGui::SliderFloat("Exposure", &fStops, -3.f, 3.f);
			m_postProConstants.exposure = powf(2.f, fStops);
		}
	}

	//---------------------------------------------------------------------------------------------------------------------
	void Renderer::createDescriptorLayouts(size_t numTextures)
	{
		// Geometry pass
		m_geometryDescriptorSets.addStorageBuffer("World mtx", 0, vk::ShaderStageFlagBits::eVertex);
		m_geometryDescriptorSets.addStorageBuffer("Materials", 1, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);

		m_geometryDescriptorSets.addTexture("IBL texture", 2, vk::ShaderStageFlagBits::eFragment);
		assert(numTextures < std::numeric_limits<uint32_t>::max());
		m_geometryDescriptorSets.addTextureArray("Textures", 3, (uint32_t)numTextures, vk::ShaderStageFlagBits::eFragment);

		constexpr uint32_t numSwapchainImages = 2;
		m_geometryDescriptorSets.close(numSwapchainImages);

		// Post process passes
		m_postProDescriptorSets.addImage("HDR Light", 0, vk::ShaderStageFlagBits::eFragment);
		m_postProDescriptorSets.close(1);
	}

	//---------------------------------------------------------------------------------------------------------------------
	void Renderer::fillConstantDescriptorSets(const SceneDesc& scene)
	{
		auto device = m_ctxt->device();

		for (int frameNdx = 0; frameNdx < 2; ++frameNdx)
		{
			gfx::DescriptorSetUpdate frameUpdate(m_geometryDescriptorSets, frameNdx);

			frameUpdate.addStorageBuffer("World mtx", m_mtxBuffers[frameNdx]);
			frameUpdate.addStorageBuffer("Materials", m_materialsBuffer);
			frameUpdate.addTexture("IBL texture", m_iblLUT);
			frameUpdate.send();

			// Material textures
			m_geometryDescriptorSets.writeArrayTextureToDescriptor(frameNdx, "Textures", scene.m_textures);
		}
	}

	//---------------------------------------------------------------------------------------------------------------------
	void Renderer::createRenderPasses()
	{
		// HDR geometry pass
		m_hdrLightPass = std::make_unique<RenderPass>(
			m_ctxt->createRenderPass({ m_hdrLightBuffer->format(), m_zBuffer->format() }),
			*m_frameBuffers);
		m_hdrLightPass->setColorTargets({ m_hdrLightBuffer.get() });
		m_hdrLightPass->setDepthTarget(*m_zBuffer);
		m_hdrLightPass->setClearDepth(0.f);
		m_hdrLightPass->setClearColor(-math::Vec4f::ones());

		// UI Render pass
		m_uiRenderPass = std::make_unique<RenderPass>(
			m_ctxt->createRenderPass({ m_ctxt->swapchainFormat() }),
			*m_frameBuffers);
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

		auto descriptorSetLayout = m_geometryDescriptorSets.layout();
		vk::PipelineLayoutCreateInfo layoutInfo({},
			1, & descriptorSetLayout, // Descriptor sets
			1, &camerasPushRange); // Push constants

		m_gbufferPipelineLayout = device.createPipelineLayout(layoutInfo);

		gfx::RasterPipeline::VertexBindings geometryBindings;
		geometryBindings.addAttribute<math::Vec3f>(0); // Vertex position
		geometryBindings.addAttribute<math::Vec3f>(1); // Normals
		geometryBindings.addAttribute<math::Vec4f>(2); // Tangents
		geometryBindings.addAttribute<math::Vec2f>(3); // UVs

		m_gBufferPipeline = std::make_unique<gfx::RasterPipeline>(
			geometryBindings,
			m_gbufferPipelineLayout,
			m_hdrLightPass->vkPass(),
			"../shaders/gbuffer.vert.spv",
			"../shaders/gbuffer.frag.spv",
			true);

		// Full screen pipeline
		vk::PushConstantRange postProPushRange(vk::ShaderStageFlagBits::eFragment, 0, sizeof(PostProPushConstants));
		auto postProDescLayout = m_postProDescriptorSets.layout();
		vk::PipelineLayoutCreateInfo postLayoutInfo({},
			1, & postProDescLayout, // Descriptor sets
			1, & postProPushRange); // Push constants
		m_postProcessPipelineLayout = device.createPipelineLayout(postLayoutInfo);
		// 
		// Intentionally empty bindings. Full screen generates all vertices on the fly
		gfx::RasterPipeline::VertexBindings fullScreenBindings;
		m_postProPipeline = std::make_unique<gfx::RasterPipeline>(
			fullScreenBindings,
			m_postProcessPipelineLayout,
			m_uiRenderPass->vkPass(),
			"../shaders/fullScreen.vert.spv",
			"../shaders/postPro.frag.spv",
			false
			);

		// Set up shader reload
		m_shaderWatcher->listen([this](auto paths) {
			m_gBufferPipeline->invalidate();
			m_postProPipeline->invalidate();
			});
	}

	//---------------------------------------------------------------------------------------------------------------------
	void Renderer::createRenderTargets()
	{
		auto windowSize = m_ctxt->windowSize();
		auto& alloc = m_ctxt->allocator();

		m_hdrLightBuffer = alloc.createImageBuffer(
			"HDR light",
			windowSize,
			vk::Format::eR32G32B32A32Sfloat,
			vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eStorage,
			m_ctxt->graphicsQueueFamily());
		m_zBuffer = alloc.createDepthBuffer(
			"Depth",
			windowSize,
			vk::Format::eD32Sfloat,
			vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferDst,
			m_ctxt->graphicsQueueFamily());

		// Transition new images to general layout
		m_ctxt->transitionImageLayout(m_hdrLightBuffer->image(), m_hdrLightBuffer->format(), vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral, false);
		m_ctxt->transitionImageLayout(m_zBuffer->image(), m_zBuffer->format(), vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral, true);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Renderer::destroyRenderTargets()
	{
		m_hdrLightBuffer = nullptr;
		m_zBuffer = nullptr;
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