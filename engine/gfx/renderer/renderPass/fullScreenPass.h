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
#pragma once

#include <memory>
#include <string_view>
#include <gfx/backend/Vulkan/renderContextVulkan.h>
#include <gfx/renderer/RenderPass.h>

namespace rev::gfx {

	class FrameBufferManager;
	class RasterPipeline;

	class FullScreenPass
	{
	public:
		FullScreenPass(
			std::string_view fragmentShader,
			vk::Format attachmentFormats,
			gfx::FrameBufferManager& fbManager,
			vk::DescriptorSetLayout descriptorSetLayout,
			size_t pushConstantsSize);
		~FullScreenPass();

		void begin(
			const vk::CommandBuffer cmd,
			const math::Vec2u& targetSize,
			const ImageBuffer& colorTargets,
			const math::Vec3f* clearColor,
			vk::DescriptorSet descriptor);

		template<class PushConstants>
		void pushConstants(vk::CommandBuffer cmd, const PushConstants& pc)
		{
			cmd.pushConstants<PushConstants>(
				m_pipelineLayout,
				vk::ShaderStageFlagBits::eFragment,
				0,
				pc);
		}

		void render(const vk::CommandBuffer cmd);
		void end(const vk::CommandBuffer cmd);

		void onResize(const math::Vec2u& size);
		void invalidateShaders();

		auto vkPass() const { return m_renderPass->vkPass(); }

	private:
		size_t m_streamingToken;
		std::unique_ptr<RenderPass> m_renderPass;
		vk::PipelineLayout m_pipelineLayout;
		std::unique_ptr<RasterPipeline> m_pipeline;
		std::shared_ptr<GPUBuffer> m_fullScreenIndexBuffer;
		math::Vec2u m_targetSize;
	};

}
