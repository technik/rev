//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2018 Carmelo J Fdez-Aguera
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

#include "fullScreenPass.h"

#include <gfx/backend/Vulkan/renderContextVulkan.h>
#include <gfx/backend/rasterPipeline.h>

namespace rev::gfx
{
	FullScreenPass::FullScreenPass(
		std::string_view fragmentShader,
		vk::Format attachmentFormats,
		gfx::FrameBufferManager& fbManager,
		vk::DescriptorSetLayout descriptorSetLayout,
		size_t pushConstantsSize
	)
	{
		m_renderPass = std::make_unique<RenderPass>(RenderContextVk().createRenderPass({ attachmentFormats }), fbManager);

		// Create shader pipeline
		auto device = RenderContextVk().nativeDevice();
		// // Full screen pipeline
		vk::PushConstantRange postProPushRange(vk::ShaderStageFlagBits::eFragment, 0, (uint32_t)pushConstantsSize);
		vk::PipelineLayoutCreateInfo postLayoutInfo({},
			1, & descriptorSetLayout, // Descriptor sets
			1, &postProPushRange); // Push constants
		m_pipelineLayout = device.createPipelineLayout(postLayoutInfo);
		
		// Intentionally empty bindings. Full screen generates all vertices on the fly
		RasterPipeline::VertexBindings fullScreenBindings;
		m_pipeline = std::make_unique<gfx::RasterPipeline>(
			fullScreenBindings,
			m_pipelineLayout,
			m_renderPass->vkPass(),
			"fullScreen.vert.spv",
			fragmentShader,
			false
			);

		// Create full screen vertices
		auto& alloc = RenderContextVk().allocator();
		alloc.reserveStreamingBuffer(3 * sizeof(uint32_t));
		m_fullScreenIndexBuffer = alloc.createGpuBuffer(
			3 * sizeof(uint32_t),
			vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
			RenderContextVk().graphicsQueueFamily());
		uint32_t indices[3] = { 0, 1, 2 };
		m_streamingToken = alloc.asyncTransfer(*m_fullScreenIndexBuffer, indices, 3);
	}

	FullScreenPass::~FullScreenPass()
	{
		m_pipeline = nullptr;
		auto device = RenderContextVk().nativeDevice();
		device.destroyPipelineLayout(m_pipelineLayout);
		device.destroyRenderPass(m_renderPass->vkPass());
	}

	void FullScreenPass::begin(
		const vk::CommandBuffer cmd,
		const math::Vec2u& targetSize,
		const ImageBuffer& colorTarget,
		const math::Vec3f* clearColor,
		vk::DescriptorSet descriptor)
	{
		m_renderPass->setColorTarget(colorTarget);
		if (clearColor)
			m_renderPass->setClearColor(*clearColor);
		else
			m_renderPass->resetClearColor();

		RenderContextVk().allocator().transitionImageLayout(cmd,
			colorTarget.image(),
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::eGeneral,
			false);
			//vk::DependencyFlagBits::eViewLocal);
		// Post-process HDR image
		m_renderPass->begin(cmd, targetSize);
		m_pipeline->bind(cmd);

		cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
			m_pipelineLayout,
			0, descriptor, {});
	}

	void FullScreenPass::render(const vk::CommandBuffer cmd)
	{
		if (!RenderContextVk().allocator().isTransferFinished(m_streamingToken))
			return;

		cmd.bindIndexBuffer(m_fullScreenIndexBuffer->buffer(), {}, vk::IndexType::eUint32);

		cmd.drawIndexed(3, 1, 0, 0, 0);
	}

	void FullScreenPass::end(const vk::CommandBuffer cmd)
	{
		m_renderPass->end(cmd);
	}

	void FullScreenPass::invalidateShaders()
	{
		m_pipeline->invalidate();
	}
}
