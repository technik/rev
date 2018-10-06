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
#include <graphics/backend/device.h>
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
		ShadowMapPass(gfx::Device& device, gfx::FrameBuffer target, const math::Vec2u& _size);

		// view camera is used to adjust projection frustum.
		// Culling BBox is extended towards the camera before culling actually happens
		void render(
			const std::vector<RenderItem>& shadowCasters,
			const std::vector<RenderItem>& shadowReceivers,
			const Camera& view,
			const Light& light);
		const math::Mat44f& shadowProj() const { return mUnbiasedShadowProj; }

	private:

		void adjustViewMatrix(const math::AffineTransform& shadowView, const math::AABB& castersBBox);
		void renderMeshes(const std::vector<gfx::RenderItem>& renderables);

		void loadCommonShaderCode();
		gfx::Pipeline getPipeline(RenderGeom::VtxFormat, bool mirroredGeometry);
		// Used to combine different vertex formats as long as they share the data needed to render shadows.
		// This saves a lot on shader permutations.
		uint32_t mVtxFormatMask;

	private:

		gfx::Device&		m_device;
		gfx::RenderPass*	m_pass;
		gfx::Pipeline::Descriptor m_commonPipelineDesc; // Config common to all shadow pipelines
		math::Mat44f		mShadowProj;
		math::Mat44f		mUnbiasedShadowProj;
		std::string			mCommonShaderCode;
		float mBias = 0.001f;

		std::unordered_map<uint32_t,gfx::Pipeline> m_regularPipelines;
		std::unordered_map<uint32_t,gfx::Pipeline> m_mirroredPipelines;
	};

}