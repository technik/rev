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

	class Camera;

	class ZPrePass
	{
	public:
		ZPrePass(Device& device, FrameBuffer target, const math::Vec2u& _size);

		static Texture2d createDepthMapTexture(Device& device, const math::Vec2u& size);
		static FrameBuffer createDepthBuffer(Device& device, Texture2d texture);

		void onResizeTarget(const math::Vec2u& newSize, Texture2d targetTexture);

		// view camera is used to adjust projection frustum.
		// Culling BBox is extended towards the camera before culling actually happens
		void render(
			const Camera& eye,
			const std::vector<gfx::RenderItem>& renderables,
			CommandBuffer& dst
		);

	private:
		void createRenderPass(const math::Vec2u& _size);

		Device&		m_device;
		RenderPass*	m_pass;
		ShaderCodeFragment m_commonCode;
		Pipeline::RasterOptions m_rasterOptions;
		GeometryPass m_geomPass;
		math::Vec2u m_viewportSize;
		FrameBuffer m_frameBuffer;

		float mBias = 0.001f;
	};

}