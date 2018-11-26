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

#include <graphics/backend/frameBuffer.h>
#include <graphics/renderer/RenderItem.h>
#include <graphics/renderer/renderPass/geometryPass.h>
#include <graphics/renderer/renderPass/fullScreenPass.h>
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

		//void drawDebugUI();

	private:
		void createBuffers();
		void createRenderPasses(gfx::FrameBuffer target);
		void collapseSceneRenderables(const RenderScene&);
		Texture2d createGBufferTexture(Device& device, const math::Vec2u& size);
		void createPBRTextures(Device& device, const math::Vec2u& size);
		Texture2d createDepthTexture(Device& device, const math::Vec2u& size);
		FrameBuffer createGBuffer(Device& device, Texture2d depth, Texture2d normal);
		ShaderCodeFragment* getMaterialCode(RenderGeom::VtxFormat, const Material& material);
		std::string vertexFormatDefines(RenderGeom::VtxFormat vertexFormat);

		using RenderItem = gfx::RenderItem;

		template<class Filter> // Filter must be an operator (RenderItem) -> bool
		void cull(const std::vector<RenderItem>& from, std::vector<RenderItem>& to, const Filter&); // TODO: Cull inplace?

	private:
		Device*		m_device = nullptr;
		math::Vec2u m_viewportSize;
		FrameBuffer m_targetFb;

		// Geometry arrays
		std::vector<RenderItem> m_renderQueue;
		std::vector<RenderItem> m_visible;

		// Geometry pass
		std::map<std::string, ShaderCodeFragment*> m_materialCode;
		Pipeline::RasterOptions m_rasterOptions;
		gfx::Texture2d			m_depthTexture;
		gfx::Texture2d			m_gBufferTexture;
		gfx::Texture2d			m_albedoTexture;
		gfx::Texture2d			m_specularTexture;
		FrameBuffer				mGBuffer;
		RenderPass*				m_gBufferPass = nullptr;
		GeometryPass*			m_gPass = nullptr;
		std::unique_ptr<FullScreenPass>		m_bgPass;

		// Shadow pass
		// SSAO pass
		// Lighting pass
		Texture2d			m_brdfIbl;
		RenderPass*			m_lPass = nullptr;
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