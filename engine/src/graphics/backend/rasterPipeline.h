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
	class RasterPipeline
	{
	public:
		struct VertexBindings
		{
			template<class T> void addAttribute(uint32_t bindingPosition);

			template<> void addAttribute<math::Vec2f>(uint32_t bindingPosition)
			{
				m_attributes.emplace_back(bindingPosition, vk::Format::eR32G32Sfloat);
			}
			template<> void addAttribute<math::Vec3f>(uint32_t bindingPosition)
			{
				m_attributes.emplace_back(bindingPosition, vk::Format::eR32G32B32Sfloat);
			}
			template<> void addAttribute<math::Vec4f>(uint32_t bindingPosition)
			{
				m_attributes.emplace_back(bindingPosition, vk::Format::eR32G32B32A32Sfloat);
			}

			std::vector<std::pair<uint32_t, vk::Format>> m_attributes;
		};

		RasterPipeline(
			const VertexBindings& vtxFormat,
			vk::PipelineLayout layout,
			vk::RenderPass passDesc,
			std::string vtxShaderFilename,
			std::string pxlShaderFilename,
			bool depthTest,
			bool blend = false
		);

		~RasterPipeline();

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
		std::vector<std::pair<uint32_t, vk::Format>> m_attributes;
		vk::PipelineLayout m_layout;
		vk::RenderPass m_passDesc;
		bool m_depthTest;
		bool m_blend;

		std::string m_vtxShader;
		std::string m_pxlShader;

		vk::Pipeline m_vkPipeline;
		bool m_invalidated = false;
	};
}
