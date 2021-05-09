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

#include <graphics/backend/Vulkan/renderContextVulkan.h>

#include <vector>

namespace rev
{
	struct RenderPass
	{
		vk::RenderPass m_vkPass;
		vk::Framebuffer m_fb;

		std::vector<vk::Image> colorTargets;
		std::optional<vk::Image> depthTarget;

		void begin(vk::CommandBuffer cmd, const math::Vec2u& targetSize)
		{
			assert(m_clearColors.size() >= colorTargets.size());

			// Clear color attachments
			if (clearColor)
			{
				for (uint32_t i = 0; i < colorTargets.size(); ++i)
				{
					auto clearColorRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
					cmd.clearColorImage(
						colorTargets[i],
						vk::ImageLayout::eGeneral,
						m_clearColors[i],
						clearColorRange);
				}
			}

			// Clear depth
			if (clearZ)
			{
				assert(depthTarget.has_value());

				auto clearDepthRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1);
				cmd.clearDepthStencilImage(
					depthTarget.value(),
					vk::ImageLayout::eGeneral,
					m_clearDepth,
					clearDepthRange);
			}

			// Bind pass
			vk::RenderPassBeginInfo passInfo;
			passInfo.framebuffer = m_fb;
			passInfo.renderPass = m_vkPass;
			passInfo.renderArea.offset.x = 0;
			passInfo.renderArea.offset.y = 0;
			passInfo.renderArea.extent.width = targetSize.x();
			passInfo.renderArea.extent.height = targetSize.y();
			passInfo.clearValueCount = 0;
			cmd.beginRenderPass(passInfo, vk::SubpassContents::eInline);

			// Update drawing space
			vk::Viewport viewport;
			viewport.x = 0;
			viewport.y = 0;
			viewport.maxDepth = 1.0f;
			viewport.minDepth = 0.0f;
			viewport.width = (float)targetSize.x();
			viewport.height = (float)targetSize.y();

			cmd.setViewport(0, 1, &viewport);
			cmd.setScissor(0, passInfo.renderArea);
		}

		void setClearDepth(float depth)
		{
			clearZ = true;
			m_clearDepth = vk::ClearDepthStencilValue(0.f);
		}

		void setClearColor(const std::vector<math::Vec4f>& colors)
		{
			clearColor = true;
			m_clearColors.reserve(colors.size());
			m_clearColors.clear();

			for (auto& c : colors)
			{
				auto clearColor = vk::ClearColorValue(std::array<float, 4>{c.x(), c.y(), c.z(), c.w()});
				m_clearColors.push_back(clearColor);
			}
		}

		void setClearColor(const math::Vec4f& c)
		{
			clearColor = true;
			m_clearColors.resize(1);
			m_clearColors[0] = vk::ClearColorValue(std::array<float, 4>{c.x(), c.y(), c.z(), c.w()});
		}

		void setClearColor(const math::Vec3f& c)
		{
			clearColor = true;
			m_clearColors.resize(1);
			m_clearColors[0] = vk::ClearColorValue(std::array<float, 4>{c.x(), c.y(), c.z(), 1.f});
		}

		void end(vk::CommandBuffer cmd)
		{
			cmd.endRenderPass();
		}

		void drawGeometry(
			vk::CommandBuffer cmd,
			const game::MeshRenderer& sceneInstances,
			const gfx::RasterHeap& rasterData)
		{
			// Draw all instances in a single batch
			rasterData.bindBuffers(cmd);
			for (size_t i = 0; i < sceneInstances.numInstances(); ++i)
			{
				assert(i < std::numeric_limits<uint32_t>::max());

				auto meshNdx = sceneInstances.m_instanceMeshes[i];
				auto& mesh = sceneInstances.m_meshes[meshNdx];
				for (size_t primitiveId = mesh.first; primitiveId != mesh.second; ++primitiveId)
				{
					auto& primitive = rasterData.getPrimitiveById(primitiveId);
					cmd.drawIndexed(primitive.numIndices, 1, primitive.indexOffset, primitive.vtxOffset, (uint32_t)i);
				}
			}
		}

	private:
		bool clearColor = false;
		bool clearZ = false;

		vk::ClearDepthStencilValue m_clearDepth;
		std::vector<vk::ClearColorValue> m_clearColors;
	};
}