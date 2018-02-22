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
#include <graphics/debug/imgui.h>
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
		core::File shaderCode("shadowMap.fx");
		mShader = Shader::createShader(shaderCode.bufferAsText());
	}

	//----------------------------------------------------------------------------------------------
	void ShadowMapPass::render(const RenderScene& _scene, const math::Vec3f& lightDir)
	{
		mDepthBuffer->bind();
		glEnable(GL_DEPTH_TEST);
		glClearDepth(1.0);
		glClear(GL_DEPTH_BUFFER_BIT);

		if(!mShader)
			return;
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		Mat44f shadowViewMtx = Mat44f::identity();
		shadowViewMtx.block<3,1>(0,1) = lightDir;
		Vec3f upAxis = Vec3f {0.f, 0.f, 1.f};
		if(std::abs(upAxis.dot(lightDir)) > 0.71f)
			upAxis = Vec3f {0.f, 1.f, 0.f};
		shadowViewMtx.block<3,1>(0,2) = upAxis;
		shadowViewMtx.block<3,1>(0,0) = lightDir.cross(upAxis).normalized();

		if(ImGui::Begin("ShadowMap"))
		{
			ImGui::SliderFloat("Shadow bias", &mBias, -1.f, 1.f);
			ImGui::End();
		}

		//mShadowProj = Mat44f::identity();
		Mat44f biasMatrix = Mat44f::identity();
		biasMatrix(1,3) = -mBias;
		auto proj = math::orthographicMatrix(math::Vec2f(2.f,2.f), -2.0f, 2.0f);
		mShadowProj = proj * biasMatrix * shadowViewMtx.transpose();
		//mShadowProj = shadowViewMtx;
		mShader->bind();

		auto worldMatrix = Mat44f::identity();
		// TODO: Performance counters
		for(auto& renderable : _scene.renderables())
		{
			auto renderObj = renderable.lock();
			// Get world matrix
			worldMatrix.block<3,4>(0,0) = renderObj->transform.matrix();
			// Set up vertex uniforms
			auto wvp = mShadowProj*worldMatrix;

			// render
			for(size_t i = 0; i < renderObj->meshes.size(); ++i)
			{
				glUniformMatrix4fv(0, 1, !Mat44f::is_col_major, wvp.data());
				// Render mesh
				renderObj->meshes[i]->render();
			}
		}

		// Remove bias for later use
		mShadowProj = proj * shadowViewMtx.transpose();
	}

}}