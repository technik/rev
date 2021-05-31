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

#include <gfx/renderer/RenderPass.h>
#include <gfx/RasterHeap.h>

using namespace rev::math;

namespace rev::gfx
{
	void RenderPass::begin(vk::CommandBuffer cmd, const Vec2u& targetSize)
	{
		assert(m_clearColors.size() >= m_colorTargets.size());

		refreshFrameBuffer(targetSize); // Refresh frame buffer

		// Clear color attachments
		if (m_clearColor)
		{
			for (uint32_t i = 0; i < m_colorTargets.size(); ++i)
			{
				auto clearColorRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
				cmd.clearColorImage(
					m_colorTargets[i],
					vk::ImageLayout::eGeneral,
					m_clearColors[i],
					clearColorRange);
			}
		}

		// Clear depth
		if (m_clearZ)
		{
			assert(m_depthTarget.has_value());

			auto clearDepthRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1);
			cmd.clearDepthStencilImage(
				m_depthTarget.value(),
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

	void RenderPass::setClearDepth(float depth)
	{
		m_clearZ = true;
		m_clearDepth = vk::ClearDepthStencilValue(0.f);
	}

	void RenderPass::setClearColor(const std::vector<Vec4f>& colors)
	{
		m_clearColor = true;
		m_clearColors.reserve(colors.size());
		m_clearColors.clear();

		for (auto& c : colors)
		{
			auto clearColor = vk::ClearColorValue(std::array<float, 4>{c.x(), c.y(), c.z(), c.w()});
			m_clearColors.push_back(clearColor);
		}
	}

	void RenderPass::setClearColor(const Vec4f& c)
	{
		m_clearColor = true;
		m_clearColors.resize(1);
		m_clearColors[0] = vk::ClearColorValue(std::array<float, 4>{c.x(), c.y(), c.z(), c.w()});
	}

	void RenderPass::setClearColor(const Vec3f& c)
	{
		m_clearColor = true;
		m_clearColors.resize(1);
		m_clearColors[0] = vk::ClearColorValue(std::array<float, 4>{c.x(), c.y(), c.z(), 1.f});
	}

	void RenderPass::setColorTargets(const std::vector<const ImageBuffer*>& colorTargets)
	{
		// Invalidate frame buffer
		m_fb = vk::Framebuffer{};

		m_colorTargets.clear();
		m_colorViews.clear();

		for (auto t : colorTargets)
		{
			m_colorTargets.push_back(t->image());
			m_colorViews.push_back(t->view());
		}
	}

	void RenderPass::setColorTarget(const ImageBuffer& colorTarget)
	{
		// Invalidate frame buffer
		m_fb = vk::Framebuffer{};

		m_colorTargets.clear();
		m_colorViews.clear();

		m_colorTargets.push_back(colorTarget.image());
		m_colorViews.push_back(colorTarget.view());
	}

	void RenderPass::setDepthTarget(const ImageBuffer& depthTarget)
	{
		// Invalidate frame buffer
		m_fb = vk::Framebuffer{};

		m_depthTarget = depthTarget.image();
		m_depthView = depthTarget.view();
	}

	void RenderPass::refreshFrameBuffer(const math::Vec2u& targetSize)
	{
		if (!m_fb)
		{
			auto views = m_colorViews;
			if (m_depthView.has_value())
				views.push_back(m_depthView.value());

			m_fb = m_fbManager.get(views, m_vkPass, targetSize);
		}
	}
}