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

		createRenderTargets();
		createRenderPasses();
		createShaderPipelines();

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
		const uint32_t numDescriptors = 2 * 2; // 2 per frame * 2 frames
		vk::DescriptorPoolSize poolSize(vk::DescriptorType::eStorageBuffer, numDescriptors);

		auto poolInfo = vk::DescriptorPoolCreateInfo({}, numDescriptors, poolSize);
		m_descPool = device.createDescriptorPool(poolInfo);

		vk::DescriptorSetLayout setLayouts[2] = { m_frameDescLayout, m_frameDescLayout };
		vk::DescriptorSetAllocateInfo setInfo(m_descPool, 2, setLayouts);
		m_frameDescs = device.allocateDescriptorSets(setInfo);

		// Update descriptor sets
		vk::DescriptorBufferInfo writeBufferInfo(m_mtxBuffers[0]->buffer(), 0, m_mtxBuffers[0]->size());
		vk::WriteDescriptorSet writeInfo;
		writeInfo.dstSet = m_frameDescs[0];
		writeInfo.dstBinding = 0;
		writeInfo.dstArrayElement = 0;
		writeInfo.descriptorType = vk::DescriptorType::eStorageBuffer;
		writeInfo.descriptorCount = 1;
		writeInfo.pBufferInfo = &writeBufferInfo;
		device.updateDescriptorSets(writeInfo, {});

		writeBufferInfo.buffer = m_mtxBuffers[1]->buffer();
		writeInfo.dstSet = m_frameDescs[1];
		device.updateDescriptorSets(writeInfo, {});

		// Matrix buffer
		vk::DescriptorBufferInfo matrixBufferInfo(m_materialsBuffer->buffer(), 0, m_materialsBuffer->size());
		writeInfo.dstBinding = 1;
		writeInfo.pBufferInfo = &matrixBufferInfo;
		device.updateDescriptorSets(writeInfo, {});

		writeInfo.dstSet = m_frameDescs[0];
		device.updateDescriptorSets(writeInfo, {});

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
		m_uiRenderPass->setClearColor(m_frameConstants.ambientColor);
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
		vk::DescriptorSetLayoutBinding matricesBinding(0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eVertex);
		vk::DescriptorSetLayoutBinding materialsBinding(1, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);
		std::vector<vk::DescriptorSetLayoutBinding> bindings{ matricesBinding, materialsBinding };

		vk::DescriptorSetLayoutCreateInfo setLayoutInfo({}, bindings);
		m_frameDescLayout = device.createDescriptorSetLayout(setLayoutInfo);

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