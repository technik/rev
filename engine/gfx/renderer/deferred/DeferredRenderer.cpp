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

#include <gfx/backend/Vulkan/gpuBuffer.h>
#include <gfx/backend/rasterPipeline.h>
#include <gfx/renderer/deferred/DeferredRenderer.h>
#include <gfx/renderer/renderPass/fullScreenPass.h>
#include <gfx/renderer/RasterQueue.h>
#include <gfx/Image.h>
#include <gfx/ImGui.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_vulkan.h>
#include <imgui/backends/imgui_impl_win32.h>

namespace rev::gfx
{
	//---------------------------------------------------------------------------------------------------------------------
	DeferredRenderer::DeferredRenderer()
	{}

	//---------------------------------------------------------------------------------------------------------------------
	DeferredRenderer::~DeferredRenderer()
	{}

	//---------------------------------------------------------------------------------------------------------------------
	void DeferredRenderer::init(
		gfx::RenderContextVulkan& ctxt,
		const math::Vec2u& windowSize,
		const Budget& limits,
		const core::FolderWatcher::path& shadersFolder,
		std::shared_ptr<EnvironmentProbe> envProbe)
	{
		m_shadersFolder = shadersFolder;
		m_windowSize = windowSize;
		m_envProbe = envProbe;

		m_ctxt = &ctxt;

		m_frameBuffers = std::make_unique<gfx::FrameBufferManager>(ctxt.device());

		auto device = ctxt.device();

		// Create semaphores
		m_imageAvailableSemaphore = device.createSemaphore({});

		createRenderTargets();
		createDescriptorLayouts(limits.maxTexturesPerBatch);
		createRenderPasses();
		createShaderPipelines();
		loadIBLLUT();

		// Update descriptor sets
		fillConstantDescriptorSets();

		gfx::initImGui(m_uiRenderPass->vkPass());
	}

	//---------------------------------------------------------------------------------------------------------------------
	void DeferredRenderer::end()
	{
		destroyFrameBuffers();
		destroyRenderTargets();

		auto device = m_ctxt->device();
		m_gBufferPipeline.reset();
		device.destroyPipelineLayout(m_gbufferPipelineLayout);
		device.destroyRenderPass(m_hdrLightPass->vkPass());
		device.destroySemaphore(m_imageAvailableSemaphore);
	}

	//---------------------------------------------------------------------------------------------------------------------
	void DeferredRenderer::onResize(const math::Vec2u& intendedSize)
	{
		if (intendedSize.x() == 0 || intendedSize.y() == 0)
			return; // Avoid recreating an empty swap chain

		m_windowSize = m_ctxt->resizeSwapchain(intendedSize);
		createRenderTargets();
		
		// Update render passes
		m_hdrLightPass->setDepthTarget(*m_zBuffer);
		m_hdrLightPass->setColorTargets({ m_hdrLightBuffer.get() });

		gfx::DescriptorSetUpdate renderBufferUpdates(*m_postProDescriptors, 0);
		renderBufferUpdates.addImage("HDR Light", m_hdrLightBuffer);
		renderBufferUpdates.send();

		// Invalidate frame buffers
		m_frameBuffers->clear();
	}

	//---------------------------------------------------------------------------------------------------------------------
	void DeferredRenderer::render(SceneDesc& scene)
	{
		if (m_windowSize.x() == 0 || m_windowSize.y() == 0)
			return; // Don't try to render while minimized

		// Watch for shader reload
		m_shaderWatcher->update();

		// Render passes
		renderGeometryPass(scene);
		renderPostProPass();

		// Swapchain update
		m_ctxt->swapchainPresent();
	}

	//---------------------------------------------------------------------------------------------------------------------
	void DeferredRenderer::renderGeometryPass(SceneDesc& scene)
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

		// Bind pipeline
		m_gBufferPipeline->bind(cmd);

		// Frame set up
		cmd.pushConstants<FramePushConstants>(
			m_gbufferPipelineLayout,
			vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
			0,
			m_frameConstants);

		// Update descriptor set with this frame's constants
		cmd.bindDescriptorSets(
			vk::PipelineBindPoint::eGraphics,
			m_gbufferPipelineLayout,
			0, m_geomFrameDescriptors->getDescriptor(0), {});

		// Render opaque geometry
		std::vector<RasterQueue::Draw> draws;
		std::vector<RasterQueue::Batch> batches;
		for(const auto& queue : scene.m_opaqueGeometry)
		{
			// Get queue draw batches
			draws.clear();
			batches.clear();
			queue->getDrawBatches(draws, batches);

			for (const auto& batch : batches)
			{
				// Update descriptor set
				cmd.bindDescriptorSets(
					vk::PipelineBindPoint::eGraphics,
					m_gbufferPipelineLayout,
					1, batch.descriptorSet , {});

				cmd.bindIndexBuffer(batch.indexBuffer->buffer(), batch.indexBuffer->offset(), batch.indexType);
				cmd.bindVertexBuffers(0, {
					batch.positionBinding.first->buffer(),
					batch.normalsBinding.first->buffer(),
					batch.tangentsBinding.first->buffer(),
					batch.texCoordBinding.first->buffer()
					}, {
					batch.positionBinding.first->offset() + batch.positionBinding.second,
					batch.normalsBinding.first->offset() + batch.normalsBinding.second,
					batch.tangentsBinding.first->offset() + batch.tangentsBinding.second,
					batch.texCoordBinding.first->offset() + batch.texCoordBinding.second
					});

				for (uint32_t i = batch.firstDraw; i < batch.endDraw; ++i)
				{
					auto& draw = draws[i];
					cmd.drawIndexed(draw.numIndices, draw.numInstances, draw.indexOffset, draw.vtxOffset, draw.instanceOffset);
				}
			}

			m_doubleBufferNdx ^= 1;
		}

		m_hdrLightPass->end(cmd);
	}

	//---------------------------------------------------------------------------------------------------------------------
	void DeferredRenderer::renderPostProPass()
	{
		m_postProConstants.windowSize = math::Vec2f((float)m_windowSize.x(), (float)m_windowSize.y());
		m_postProConstants.ambientColor = m_frameConstants.ambientColor;
		m_postProConstants.renderFlags = {};
		m_postProConstants.bloom = 0.f;

		auto cmd = m_ctxt->getNewRenderCmdBuffer();
		cmd.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

		auto& swapchainImage = m_ctxt->swapchainAquireNextImage(m_imageAvailableSemaphore, cmd);

		m_ctxt->allocator().transitionImageLayout(cmd, m_hdrLightBuffer->image(), vk::ImageLayout::eGeneral, vk::ImageLayout::eShaderReadOnlyOptimal, false);
		m_uiRenderPass->begin(
			cmd,
			m_windowSize,
			swapchainImage,
			m_postProConstants.ambientColor,
			m_postProDescriptors->getDescriptor(0));

		m_uiRenderPass->pushConstants(cmd, m_postProConstants);
		m_uiRenderPass->render(cmd);

		// Finish ImGui
		//ImGui_ImplVulkan_NewFrame();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

		m_uiRenderPass->end(cmd);

		m_ctxt->allocator().transitionImageLayout(cmd, m_hdrLightBuffer->image(), vk::ImageLayout::eShaderReadOnlyOptimal, vk::ImageLayout::eGeneral, false);

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

	void DeferredRenderer::updateUI()
	{
		if (ImGui::CollapsingHeader("Renderer"))
		{
			// Render flags
			bool overrideMaterial = renderFlag(RF_OVERRIDE_MATERIAL);
			ImGui::Checkbox("Override Material", &overrideMaterial);
			bool useEnvProbe = renderFlag(RF_ENV_PROBE);
			if (m_envProbe)
			{
				ImGui::Checkbox("Env. Probe", &useEnvProbe);
			}
			else
			{
				useEnvProbe = false;
			}
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
	void DeferredRenderer::createDescriptorLayouts(size_t numTextures)
	{
		// --- Geometry pass ---
		// Geometry batches
		m_geomBatchDescriptorLayout = std::make_shared<DescriptorSetLayout>();
		m_geomBatchDescriptorLayout->addStorageBuffer("worldMtx", 0, vk::ShaderStageFlagBits::eVertex);
		m_geomBatchDescriptorLayout->addStorageBuffer("materials", 1, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);
		assert(numTextures < std::numeric_limits<uint32_t>::max());
		m_geomBatchDescriptorLayout->addTextureArray("textures", 2, (uint32_t)numTextures, vk::ShaderStageFlagBits::eFragment);
		m_geomBatchDescriptorLayout->close();

		// Frame constants
		m_geomFrameDescriptorLayout = std::make_shared<DescriptorSetLayout>();
		m_geomFrameDescriptorLayout->addTexture("iblLUT", 0, vk::ShaderStageFlagBits::eFragment);
		m_geomFrameDescriptorLayout->addTexture("envProbe", 1, vk::ShaderStageFlagBits::eFragment);
		m_geomFrameDescriptorLayout->close();
		m_geomFrameDescriptors = std::make_shared <DescriptorSetPool>(m_geomFrameDescriptorLayout, 1);

		// Post process passes
		m_postProDescriptorLayout = std::make_shared<DescriptorSetLayout>();
		m_postProDescriptorLayout->addImage("HDR Light", 0, vk::ShaderStageFlagBits::eFragment);
		m_geomFrameDescriptorLayout->addTexture("envProbe", 1, vk::ShaderStageFlagBits::eFragment);
		m_postProDescriptorLayout->close();
		m_postProDescriptors = std::make_shared <DescriptorSetPool>(m_postProDescriptorLayout, 1);
	}

	//---------------------------------------------------------------------------------------------------------------------
	void DeferredRenderer::fillConstantDescriptorSets()
	{
		auto device = m_ctxt->device();

		// Geometry
		gfx::DescriptorSetUpdate geomFrameUpdates(*m_geomFrameDescriptors, 0);
		geomFrameUpdates.addTexture("iblLUT", m_iblLUT);
		if (m_envProbe)
			geomFrameUpdates.addTexture("envProbe", m_envProbe->texture());
		geomFrameUpdates.send();

		// HDR light
		gfx::DescriptorSetUpdate renderBufferUpdates(*m_postProDescriptors, 0);
		renderBufferUpdates.addImage("HDR Light", m_hdrLightBuffer);
		if (m_envProbe)
			geomFrameUpdates.addTexture("envProbe", m_envProbe->texture());
		renderBufferUpdates.send();
	}

	//---------------------------------------------------------------------------------------------------------------------
	void DeferredRenderer::createRenderPasses()
	{
		// HDR geometry pass
		m_hdrLightPass = std::make_unique<gfx::RenderPass>(
			m_ctxt->createRenderPass({ m_hdrLightBuffer->format(), m_zBuffer->format() }),
			*m_frameBuffers);
		m_hdrLightPass->setColorTargets({ m_hdrLightBuffer.get() });
		m_hdrLightPass->setDepthTarget(*m_zBuffer);
		m_hdrLightPass->setClearDepth(0.f);
		m_hdrLightPass->setClearColor(-math::Vec4f::ones());

		// UI Render pass
		m_uiRenderPass = std::unique_ptr<gfx::FullScreenPass>(new gfx::FullScreenPass(
			"../shaders/postPro.frag.spv",
			{ m_ctxt->swapchainFormat() },
			* m_frameBuffers,
			m_postProDescriptorLayout->layout(),
			sizeof(PostProPushConstants)));
	}

	//---------------------------------------------------------------------------------------------------------------------
	void DeferredRenderer::createShaderPipelines()
	{
		auto device = m_ctxt->device();

		m_shaderWatcher = std::make_unique<core::FolderWatcher>(m_shadersFolder);

		// G-Buffer pipeline
		vk::PushConstantRange camerasPushRange(
			vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
			0, sizeof(FramePushConstants));

		vk::DescriptorSetLayout descriptorSetLayouts[2] = { m_geomFrameDescriptorLayout->layout(), m_geomBatchDescriptorLayout->layout() };
		vk::PipelineLayoutCreateInfo layoutInfo({},
			2, descriptorSetLayouts, // Descriptor sets
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
			"gbuffer.vert.spv",
			"gbuffer.frag.spv",
			true);

		// Set up shader reload
		m_shaderWatcher->listen([this](auto paths) {
			m_gBufferPipeline->invalidate();
			m_uiRenderPass->invalidateShaders();
			});
	}

	//---------------------------------------------------------------------------------------------------------------------
	void DeferredRenderer::createRenderTargets()
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
		m_ctxt->transitionImageLayout(m_hdrLightBuffer->image(), vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral, false);
		m_ctxt->transitionImageLayout(m_zBuffer->image(), vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral, true);
	}

	//------------------------------------------------------------------------------------------------------------------
	void DeferredRenderer::destroyRenderTargets()
	{
		m_hdrLightBuffer = nullptr;
		m_zBuffer = nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------
	void DeferredRenderer::destroyFrameBuffers()
	{
		m_frameBuffers->clear();
	}

	void DeferredRenderer::loadIBLLUT()
	{
		auto image = gfx::Image4f::load("ibl_brdf.hdr");
		m_iblLUT = m_ctxt->allocator().createTexture(
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
}