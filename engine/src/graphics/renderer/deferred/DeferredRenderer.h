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

#include <graphics/backend/gpuTypes.h>
#include <graphics/renderer/RenderItem.h>
#include <graphics/renderer/renderPass/geometryPass.h>
#include <graphics/renderer/renderPass/fullScreenPass.h>
#include <graphics/renderer/ShadowMapPass.h>
#include <graphics/renderGraph/renderGraph.h>
#include <graphics/renderGraph/frameBufferCache.h>
#include <random>
#include <vector>

namespace rev::gfx {

	class Camera;
	class Device;
	class RenderPass;
	class RenderScene;

	class DeferredRenderer
	{
	public:
		void init(Device& device, const math::Vec2u& _size, FrameBuffer target);

		void render	(const RenderScene&, const Camera& pov);
		void onResizeTarget(const math::Vec2u& _newSize);

	private:
		void createRenderPasses(gfx::FrameBuffer target);
		void loadNoiseTextures();
		void collapseSceneRenderables(const RenderScene&);
		void sortVisibleQueue();

	private:
		Device*		m_device = nullptr;

		// Debug utils
		bool m_lockCulling = false;

		// Render state
		math::Frustum m_cullingFrustum;
		math::Mat44f m_cullingViewMtx;
		math::AABB m_visibleVolume;
		float m_expositionValue = 0.f;
		math::Vec2u m_viewportSize;
		math::Vec2u m_shadowSize;
		FrameBuffer m_targetFb;
		std::unique_ptr<FrameBufferCache> m_fbCache;

		// Noise
		static constexpr unsigned NumBlueNoiseTextures = 64;
		rev::gfx::Texture2d m_blueNoise[NumBlueNoiseTextures];
		std::default_random_engine m_rng;
		std::uniform_int_distribution<unsigned> m_noisePermutations;

		// Geometry arrays
		std::vector<RenderItem> m_renderQueue;
		std::vector<RenderItem> m_visibleQueue;
		std::vector<RenderItem> m_opaqueQueue;
		std::vector<RenderItem> m_alphaMaskQueue;
		std::vector<RenderItem> m_emissiveQueue;
		std::vector<RenderItem> m_emissiveMaskQueue;
		std::vector<RenderItem> m_transparentQueue;

		// Geometry pass
		Pipeline::RasterOptions m_rasterOptions;
		std::unique_ptr<GeometryPass>	m_gBufferPass = nullptr;
		std::unique_ptr<GeometryPass>	m_gBufferMaskedPass = nullptr;
		std::unique_ptr<GeometryPass>	m_gTransparentPass = nullptr;
		std::unique_ptr<FullScreenPass>	m_bgPass;
		std::unique_ptr<FullScreenPass>	m_hdrPass;
		std::unique_ptr<FullScreenPass>	m_aoSamplePass;
		std::unique_ptr<ShadowMapPass>	m_shadowPass;

		// Shadow pass
		// SSAO pass
		// Lighting pass
		Texture2d			m_brdfIbl;
		FullScreenPass*		m_lightingPass = nullptr;

		// Shadow map(s)
		// SSAO map F32 / RGB32
		// SSAO blur map
		// G-Buffer:
		//	Normals buffer -> RGB32
		//	Depth map -> F32
		//	Albedo, metallic, roughness -> RGB8
		//	Emissive -> RGB32
	};

}	// namespace rev::gfx