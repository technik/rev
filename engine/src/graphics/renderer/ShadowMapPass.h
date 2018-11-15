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
#include <math/algebra/affineTransform.h>
#include <math/geometry/aabb.h>
#include <graphics/backend/commandBuffer.h>
#include <graphics/renderer/renderPass/geometryPass.h>
#include <graphics/shaders/shaderCodeFragment.h>
#include <graphics/scene/renderGeom.h>
#include <graphics/scene/renderObj.h>
#include <graphics/scene/Light.h>
#include <graphics/renderer/RenderItem.h>
#include <unordered_map>
#include <vector>

namespace rev::gfx {

	class BackEndRenderer;
	class Camera;
	class GraphicsDriverGL;
	class RenderTarget;

	class ShadowMapPass
	{
	public:
		ShadowMapPass(Device& device, FrameBuffer target, const math::Vec2u& _size);

		static Texture2d createShadowMapTexture(Device& device, const math::Vec2u& size);
		static FrameBuffer createShadowBuffer(Device& device, Texture2d texture);

		// view camera is used to adjust projection frustum.
		// Culling BBox is extended towards the camera before culling actually happens
		void render(
			const std::vector<RenderItem>& shadowCasters,
			const std::vector<RenderItem>& shadowReceivers,
			const Camera& view,
			const Light& light);
		const math::Mat44f& shadowProj() const { return mUnbiasedShadowProj; }
		void submit();

		float& bias() { return mBias; }

	private:

		void adjustViewMatrix(const math::AffineTransform& shadowView, const math::AABB& castersBBox);
		void renderMeshes(const std::vector<gfx::RenderItem>& renderables);

	private:
		Device&		m_device;
		RenderPass*	m_pass;
		CommandBuffer m_commands;
		ShaderCodeFragment m_commonCode;
		Pipeline::RasterOptions m_rasterOptions;
		GeometryPass m_geomPass;

		math::Mat44f		mShadowProj;
		math::Mat44f		mUnbiasedShadowProj;
		float mBias = 0.001f;
	};

}