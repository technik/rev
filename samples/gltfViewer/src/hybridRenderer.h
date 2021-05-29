//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2019 Carmelo J Fdez-Aguera
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

#include <math/algebra/vector.h>
#include <gfx/backend/frameBuffer.h>
#include <gfx/scene/renderGeom.h>
#include <vector>

namespace rev::gfx {

	class Camera;
	class Device;
	class RenderPass;
	class RenderScene;
}

class HybridRenderer
{
public:
	struct RigidGeometry
	{
		int m_rtGeom;// RT Geometry
		rev::gfx::RenderGeom m_rasterGeom; // Raster Geometry
	};

	struct RigidRenderObj
	{};

	struct Scene
	{
	public:
		void rebuildBVH();
		void refitBVH();

	private:
		std::vector<RigidRenderObj> m_visibleObjects;// Vector visible objects
	};

	void init(rev::gfx::Device& device, const rev::math::Vec2u& _size, rev::gfx::FrameBuffer target);

	int registerGeometry();

	void render(const Scene& scene, const rev::gfx::Camera& pov);
	void onResizeTarget(const rev::math::Vec2u& _newSize);

private:
	rev::gfx::Device*		m_device = nullptr;
	rev::math::Vec2u		m_viewportSize;
	rev::gfx::FrameBuffer	m_targetFb;
};