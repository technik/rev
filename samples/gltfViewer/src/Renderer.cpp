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

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_vulkan.h>
#include <imgui/backends/imgui_impl_win32.h>

// Imgui windows handler
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace rev
{
	//---------------------------------------------------------------------------------------------------------------------
	void Renderer::init(gfx::RenderContextVulkan& ctxt, const math::Vec2u& windowSize, size_t maxNumInstances)
	{
		m_windowSize = windowSize;
		m_maxNumInstances = maxNumInstances;

		m_ctxt = &ctxt;

		auto device = ctxt.device();

		// Create semaphores
		m_imageAvailableSemaphore = device.createSemaphore({});

		// Init descriptor sets
		const uint32_t numDescriptors = 2 * 2; // 2 per frame * 2 frames
		vk::DescriptorPoolSize poolSize(vk::DescriptorType::eStorageBuffer, numDescriptors);

		auto poolInfo = vk::DescriptorPoolCreateInfo({}, numDescriptors, poolSize);
		m_descPool = device.createDescriptorPool(poolInfo);

		vk::DescriptorSetLayoutBinding matricesBinding(0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eVertex);

		vk::DescriptorSetLayoutCreateInfo setLayoutInfo({}, matricesBinding);
		m_frameDescLayout = device.createDescriptorSetLayout(setLayoutInfo);
		vk::DescriptorSetLayout setLayouts[2] = { m_frameDescLayout, m_frameDescLayout };

		vk::DescriptorSetAllocateInfo setInfo(m_descPool, 2, setLayouts);
		m_frameDescs = device.allocateDescriptorSets(setInfo);

		// UI pipeline layout
		vk::PushConstantRange camerasPushRange(
			vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
			0, sizeof(FramePushConstants));

		vk::PipelineLayoutCreateInfo layoutInfo({},
			1, &m_frameDescLayout, // Descriptor sets
			1, &camerasPushRange); // Push constants

		m_gbufferPipelineLayout = device.createPipelineLayout(layoutInfo);

		// UI Render pass
		m_uiPass = ctxt.createRenderPass({
			{ ctxt.swapchainFormat(), vk::ImageLayout::eGeneral, vk::ImageLayout::eGeneral },
			{ vk::Format::eD32Sfloat, vk::ImageLayout::eGeneral, vk::ImageLayout::eGeneral }
			});

		createFrameBuffers();

		m_gBufferPipeline = std::make_unique<gfx::RasterPipeline>(
			device,
			m_gbufferPipelineLayout,
			m_uiPass,
			"../shaders/gbuffer.vert.spv",
			"../shaders/gbuffer.frag.spv",
			true);
		m_shaderWatcher = std::make_unique<core::FolderWatcher>(core::FolderWatcher::path("../shaders"));
		m_shaderWatcher->listen([this](auto paths) { m_gBufferPipeline->invalidate(); });

		// Allocate matrix buffers
		auto& alloc = m_ctxt->allocator();
		m_mtxBuffers.resize(2);
		m_mtxBuffers[0] = alloc.createBufferForMapping(sizeof(math::Mat44f) * maxNumInstances, vk::BufferUsageFlagBits::eStorageBuffer, m_ctxt->graphicsQueueFamily());
		m_mtxBuffers[1] = alloc.createBufferForMapping(sizeof(math::Mat44f) * maxNumInstances, vk::BufferUsageFlagBits::eStorageBuffer, m_ctxt->graphicsQueueFamily());

		// Update descriptor sets
		vk::WriteDescriptorSet writeInfo;
		writeInfo.dstSet = m_frameDescs[0];
		writeInfo.dstBinding = 0;
		writeInfo.dstArrayElement = 0;
		writeInfo.descriptorType = vk::DescriptorType::eStorageBuffer;
		writeInfo.descriptorCount = 1;
		vk::DescriptorBufferInfo writeBufferInfo(m_mtxBuffers[0]->buffer(), 0, m_mtxBuffers[0]->size());
		writeInfo.pBufferInfo = &writeBufferInfo;
		device.updateDescriptorSets(writeInfo, {});
		writeInfo.dstSet = m_frameDescs[1];
		writeBufferInfo.buffer = m_mtxBuffers[1]->buffer();
		device.updateDescriptorSets(writeInfo, {});

		initImGui();
	}

	//---------------------------------------------------------------------------------------------------------------------
	void Renderer::end()
	{
		destroyFrameBuffers();

		auto device = m_ctxt->device();
		m_gBufferPipeline.reset();
		device.destroyPipelineLayout(m_gbufferPipelineLayout);
		device.destroyRenderPass(m_uiPass);
		device.destroySemaphore(m_imageAvailableSemaphore);
	}

	//---------------------------------------------------------------------------------------------------------------------
	void Renderer::onResize(const math::Vec2u& windowSize)
	{
		m_windowSize = windowSize;
		m_ctxt->resizeSwapchain(windowSize);
		createFrameBuffers();
	}

	//---------------------------------------------------------------------------------------------------------------------
	void Renderer::render(SceneDesc& scene, bool geometryReady)
	{
		// Watch for shader reload
		m_shaderWatcher->update();
		m_frameConstants.lightColor = scene.lightColor;
		m_frameConstants.ambientColor = scene.ambientColor;
		m_frameConstants.lightDir = normalize(scene.lightDir);

		auto cmd = m_ctxt->getNewRenderCmdBuffer();

		// Record frame
		cmd.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

		// Clear
		auto clearColor = vk::ClearColorValue(std::array<float, 4>{
			m_frameConstants.ambientColor.x(),
			m_frameConstants.ambientColor.y(),
			m_frameConstants.ambientColor.z(),
			1.f });

		auto swapchainImage = m_ctxt->swapchainAquireNextImage(m_imageAvailableSemaphore, cmd);

		auto clearColorRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
		cmd.clearColorImage(
			swapchainImage,
			vk::ImageLayout::eGeneral,
			clearColor,
			clearColorRange);

		auto clearDepthRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1);
		auto clearDepth = vk::ClearDepthStencilValue(0.f);
		cmd.clearDepthStencilImage(
			m_gBufferZ->image(),
			vk::ImageLayout::eGeneral,
			clearDepth,
			clearDepthRange);

		// Render the UI pass
		vk::RenderPassBeginInfo passInfo;
		passInfo.framebuffer = m_swapchainFrameBuffers[m_ctxt->currentFrameIndex()];
		passInfo.renderPass = m_uiPass;
		passInfo.renderArea.offset;
		passInfo.renderArea.extent.width = m_ctxt->windowSize().x();
		passInfo.renderArea.extent.height = m_ctxt->windowSize().y();
		passInfo.clearValueCount = 0;
		cmd.beginRenderPass(passInfo, vk::SubpassContents::eInline);

		// Render a triangle if the scene is loaded
		if (geometryReady)
		{
			m_gBufferPipeline->bind(cmd);
			vk::Viewport viewport{ {0,0} };
			viewport.maxDepth = 1.0f;
			viewport.minDepth = 0.0f;
			viewport.width = (float)passInfo.renderArea.extent.width;
			viewport.height = (float)passInfo.renderArea.extent.height;
			cmd.setViewport(0, 1, &viewport);
			cmd.setScissor(0, passInfo.renderArea);

			// Update per instance model matrices
			auto mtxDst = m_ctxt->allocator().mapBuffer<math::Mat44f>(*m_mtxBuffers[m_doubleBufferNdx]);
			scene.m_sceneInstances.updatePoses(mtxDst);
			m_ctxt->allocator().unmapBuffer(mtxDst);

			// Update descriptor set with this frame's matrices
			cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_gbufferPipelineLayout, 0, m_frameDescs[m_doubleBufferNdx], {});

			// Update view and projection matrices push constants
			float aspect = float(m_windowSize.x()) / m_windowSize.y();
			m_frameConstants.proj = scene.proj;
			m_frameConstants.view = scene.view;

			cmd.pushConstants<FramePushConstants>(
				m_gbufferPipelineLayout,
				vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
				0,
				m_frameConstants);

			// Draw all instances in a single batch
			scene.m_rasterData.bindBuffers(cmd);
			for (size_t i = 0; i < scene.m_sceneInstances.numInstances(); ++i)
			{
				assert(i < std::numeric_limits<uint32_t>::max());

				auto meshNdx = scene.m_sceneInstances.m_instanceMeshes[i];
				auto& mesh = scene.m_sceneInstances.m_meshes[meshNdx];
				for (size_t primitiveId = mesh.first; primitiveId != mesh.second; ++primitiveId)
				{
					auto& primitive = scene.m_rasterData.getPrimitiveById(primitiveId);
					cmd.drawIndexed(primitive.numIndices, 1, primitive.indexOffset, primitive.vtxOffset, (uint32_t)i);
				}
			}

			m_doubleBufferNdx ^= 1;

		}

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

		cmd.endRenderPass();

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
	void Renderer::createFrameBuffers()
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

		// Actual frame buffer creation
		m_swapchainFrameBuffers.clear();

		// Create frame buffers for each image in the swapchain
		m_swapchainFrameBuffers.reserve(m_ctxt->nSwapChainImages());
		for (size_t i = 0; i < m_ctxt->nSwapChainImages(); ++i) {
			std::vector<vk::ImageView> imageViews = { m_ctxt->swapchainImageView(i), m_gBufferZ->view() };
			auto fbInfo = vk::FramebufferCreateInfo({},
				m_uiPass,
				imageViews,
				(uint32_t)windowSize.x(), (uint32_t)windowSize.y(), 1);
			m_swapchainFrameBuffers.push_back(m_ctxt->device().createFramebuffer(fbInfo));
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void Renderer::destroyFrameBuffers()
	{
		m_swapchainFrameBuffers.clear();

		m_gBufferNormals = nullptr;
		m_gBufferPBR = nullptr;
		m_gBufferZ = nullptr;
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

		ImGui_ImplVulkan_Init(&initInfo, m_uiPass);

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