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
#pragma once

#include "Vulkan/renderContextVulkan.h"

#include <string>
#include <vector>

namespace rev::gfx
{
	class ComputePipeline
	{
	public:
		ComputePipeline(
			vk::PipelineLayout layout,
			std::string_view shaderFilename
		);

		~ComputePipeline();

		void bind(const vk::CommandBuffer& cmdBuf);
		void bindDescriptorSets(
			const vk::CommandBuffer& cmdBuf,
			const vk::ArrayProxy<const vk::DescriptorSet>& descSets,
			uint32_t firstSet = 0);

		bool reload();
		void invalidate()
		{
			m_invalidated = true;
		}

	private:
		void clearPipeline();
		vk::Pipeline tryLoad();
		vk::ShaderModule loadShaderModule(const std::string& fileName);

	private:
		// Permanent state
		vk::PipelineLayout m_layout;
		std::string m_shader;

		vk::Pipeline m_vkPipeline;
		bool m_invalidated = false;
	};
}
