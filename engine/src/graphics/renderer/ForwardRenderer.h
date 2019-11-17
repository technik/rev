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
#include "ForwardPass.h"
#include "ShadowMapPass.h"
#include <graphics/backend/device.h>
#include <graphics/backend/texture2d.h>
#include <graphics/backend/gpuTypes.h>
#include <graphics/renderer/RenderItem.h>
#include <graphics/renderer/renderPass/fullScreenPass.h>
#include <graphics/renderer/renderPass/zPrePass.h>

namespace rev::gfx {

	class Camera;
	class RenderScene;
	class RenderTarget;

	class ForwardRenderer
	{
	public:
		void init	(gfx::Device& device, const math::Vec2u& targetSize, gfx::FrameBuffer& target);
		void render	(const RenderScene&, const Camera& pov);
		void onResizeTarget(const math::Vec2u& _newSize);

		void drawDebugUI();

		float& shadowBias() { return mShadowPass->bias(); }

	private:
		void collapseSceneRenderables(const RenderScene&);
		void initBackgroundPass(gfx::Device& device, const math::Vec2u& targetSize);

		using RenderItem = gfx::RenderItem;

		template<class Filter> // Filter must be an operator (RenderItem) -> bool
		void cull(const std::vector<RenderItem>& from, std::vector<RenderItem>& to, const Filter&); // TODO: Cull inplace?

	private:
		math::Vec2u m_targetSize;

		gfx::Texture2d						m_shadowsTexture;
		gfx::Texture2d						m_depthTexture;
		std::unique_ptr<ShadowMapPass>		mShadowPass;
		std::unique_ptr<ForwardPass>		mForwardPass;
		std::unique_ptr<ZPrePass>			mZPrePass;
		std::unique_ptr<FullScreenPass>		m_bgPass;
		gfx::FrameBuffer					m_targetBuffer;

		// Renderable queues
		gfx::Device* m_device = nullptr;
		std::vector<RenderItem> m_renderQueue;
		std::vector<RenderItem> m_visible;
		std::vector<RenderItem> m_sdwCasters;
		std::vector<RenderItem> m_visibleReceivers;
	};

}
