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

#include <gfx/backend/Vulkan/renderContextVulkan.h>
#include <gfx/backend/FrameBufferManager.h>

#include <vector>

namespace rev::gfx
{
	class RasterHeap;

	class RenderPass
	{
	public:
		RenderPass(vk::RenderPass vkPass, gfx::FrameBufferManager& fbManager)
			: m_vkPass(vkPass)
			, m_fbManager(fbManager)
		{}

		virtual ~RenderPass() = default;

		vk::RenderPass vkPass() const { return m_vkPass; }

		void begin(vk::CommandBuffer cmd, const math::Vec2u& targetSize);

		void resetClearDepth() { m_clearZ = false; }
		void setClearDepth(float depth);

		void resetClearColor() { m_clearColor = false; }
		void setClearColor(const std::vector<math::Vec4f>& colors);
		void setClearColor(const math::Vec4f& c);
		void setClearColor(const math::Vec3f& c);

		void setColorTargets(const std::vector<const gfx::ImageBuffer*>& colorTargets);
		void setColorTarget(const gfx::ImageBuffer& colorTarget); // Specialization for single target

		void setDepthTarget(const gfx::ImageBuffer& depthTarget);

		void end(vk::CommandBuffer cmd) { cmd.endRenderPass(); }

	private:

		void refreshFrameBuffer(const math::Vec2u& targetSize);

		vk::RenderPass m_vkPass;
		gfx::FrameBufferManager& m_fbManager;

		bool m_clearColor = false;
		bool m_clearZ = false;

		std::vector<vk::Image> m_colorTargets;
		std::vector<vk::ImageView> m_colorViews;
		std::optional<vk::Image> m_depthTarget;
		std::optional<vk::ImageView> m_depthView;

		vk::Framebuffer m_fb;
		vk::ClearDepthStencilValue m_clearDepth;
		std::vector<vk::ClearColorValue> m_clearColors;
	};
}