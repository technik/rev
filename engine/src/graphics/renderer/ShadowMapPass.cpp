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
#include "ShadowMapPass.h"

#include <core/platform/fileSystem/file.h>
#include <graphics/driver/openGL/GraphicsDriverOpenGL.h>
#include <graphics/driver/shader.h>
#include <graphics/scene/camera.h>
#include <graphics/scene/renderGeom.h>
#include <graphics/scene/renderObj.h>
#include <graphics/scene/renderScene.h>
#include <math/algebra/affineTransform.h>

using namespace rev::math;

namespace rev { namespace graphics {

	//----------------------------------------------------------------------------------------------
	ShadowMapPass::ShadowMapPass(GraphicsDriverGL& _gfxDriver, const Vec2u& _size)
		: mDriver(_gfxDriver)
	{
		mDepthBuffer = std::make_unique<FrameBuffer>(_size);
	}

	//----------------------------------------------------------------------------------------------
	void ShadowMapPass::render(const RenderScene& _scene, const math::Vec3f& lightDir)
	{
		mDepthBuffer->bind();
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glClearDepth(1.0);
		glClear(GL_DEPTH_BUFFER_BIT);

		auto vp = math::orthographicMatrix(math::Vec2f(2.f,2.f), 0.f, 4.f);

		auto worldMatrix = Mat44f::identity();
		// TODO: Performance counters
		for(auto& renderable : _scene.renderables())
		{
			auto renderObj = renderable.lock();
			// Get world matrix
			worldMatrix.block<3,4>(0,0) = renderObj->transform.matrix();
			// Set up vertex uniforms
			auto wvp = vp*worldMatrix;

			// render
			for(size_t i = 0; i < renderObj->meshes.size(); ++i)
			{
				glUniformMatrix4fv(0, 1, !Mat44f::is_col_major, wvp.data());
				// Render mesh
				renderObj->meshes[i]->render();
			}
		}
	}

}}