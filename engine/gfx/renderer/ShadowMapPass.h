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
#include <graphics/renderer/renderPass/geometryPass.h>
#include <graphics/shaders/shaderCodeFragment.h>
#include <graphics/scene/renderGeom.h>
#include <graphics/scene/renderObj.h>
#include <graphics/scene/Light.h>
#include <graphics/renderer/RenderItem.h>
#include <unordered_map>
#include <vector>

namespace rev::gfx {

	/*class Camera;

	class ShadowMapPass
	{
	public:
		ShadowMapPass(Device& device, const math::Vec2u& _size);
		~ShadowMapPass();

		//static Texture2d createShadowMapTexture(Device& device, const math::Vec2u& size);
		//static FrameBuffer createShadowBuffer(Device& device, Texture2d texture);

		// view camera is used to adjust projection frustum.
		// Culling BBox is extended towards the camera before culling actually happens
		void render(
			const std::vector<RenderItem>& shadowCasters,
			const math::AABB& shadowReceiversViewSpaceBBox,
			float aspectRatio,
			const Camera& view,
			const Light& light,
			CommandBuffer& dst);
		const math::Mat44f& shadowProj() const { return mUnbiasedShadowProj; }

		float& bias() { return mBias; }

	private:

		void adjustViewMatrix(const math::Mat44f& shadowView, const math::AABB& castersBBox);
		void renderMeshes(const std::vector<gfx::RenderItem>& renderables, CommandBuffer& dst);
		void reserveMatrixBuffer(size_t numObjects);

	private:
		Device&		m_device;
		//RenderPass*	m_pass;
		Pipeline::RasterOptions m_rasterOptions;
		GeometryPass m_geomPass;
		std::vector<RenderItem> m_visibleCasters;
		CommandBuffer::UniformBucket m_passWideSSBOs;

		math::Mat44f		mShadowProj;
		math::Mat44f		mUnbiasedShadowProj;
		float mMaxShadowDistance = 100.f;
		float mBias = 0.001f;

		// Draw batches
		std::vector<CommandBuffer::BatchCommand> m_batches;

		// GPU data
		size_t m_gpuMatrixCapacity = 0;
		Buffer m_gpuMatrixBuffer;
		Buffer m_gpuBatchCommands;
	};*/

}