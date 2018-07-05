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
#include <graphics/renderer/backend/BackEndRenderer.h>
#include <graphics/driver/shader.h>
#include <graphics/scene/camera.h>
#include <graphics/scene/renderGeom.h>
#include <graphics/scene/renderMesh.h>
#include <graphics/scene/renderObj.h>
#include <graphics/scene/renderScene.h>
#include <math/algebra/affineTransform.h>

using namespace rev::math;

namespace rev { namespace graphics {

	//----------------------------------------------------------------------------------------------
	ShadowMapPass::ShadowMapPass(BackEndRenderer& _backEnd, GraphicsDriverGL& _gfxDriver, const Vec2u& _size)
		: mDriver(_gfxDriver)
		, mBackEnd(_backEnd)
	{
		mDepthBuffer = std::make_unique<FrameBuffer>(_size);
		mVtxFormatMask = RenderGeom::VtxFormat(
			RenderGeom::VtxFormat::Storage::Float32,
			RenderGeom::VtxFormat::Storage::None,
			RenderGeom::VtxFormat::Storage::None,
			RenderGeom::VtxFormat::Storage::None
		).code(); // We only care about the format of vertex positions

		loadCommonShaderCode();
	}

	//----------------------------------------------------------------------------------------------
	void ShadowMapPass::render(const RenderScene& _scene)
	{
		setUpGlobalState(); // Set gl global state
		// TODO: Cull shadow casters
		adjustViewMatrix(_scene.lights()[0]->viewDirection());// Adjust view matrix
		// Iterate over renderables
		renderMeshes(_scene);

		
	}

	//----------------------------------------------------------------------------------------------
	void ShadowMapPass::setUpGlobalState()
	{
		mDepthBuffer->bind();
		glEnable(GL_DEPTH_TEST);
		glClearDepthf(1.0);
		glClear(GL_DEPTH_BUFFER_BIT);

		glDepthFunc(GL_LEQUAL);
		glEnable(GL_CULL_FACE);
	}

	//----------------------------------------------------------------------------------------------
	void ShadowMapPass::adjustViewMatrix(const math::Vec3f& lightDir)
	{
		Mat44f shadowViewMtx = Mat44f::identity();
		shadowViewMtx.block<3,1>(0,1) = Vec3f(0.f,0.f,1.f);//lightDir;
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

		Mat44f biasMatrix = Mat44f::identity();
		biasMatrix(1,3) = -mBias;
		auto proj = math::orthographicMatrix(math::Vec2f(2.f,2.f), -2.0f, 2.0f);
		//mShadowProj = proj * shadowViewMtx.transpose();
		mShadowProj = proj * biasMatrix * shadowViewMtx.transpose();
		mUnbiasedShadowProj = proj * shadowViewMtx.transpose();
		//mUnbiasedShadowProj = mShadowProj;
	}

	//----------------------------------------------------------------------------------------------
	void ShadowMapPass::renderMeshes(const RenderScene& _scene)
	{
		auto worldMatrix = Mat44f::identity();
		// TODO: Performance counters
		mBackEnd.beginPass();

		for(auto& renderable : _scene.renderables())
		{
			auto renderObj = renderable;
			// Get world matrix
			worldMatrix.block<3,4>(0,0) = renderObj->transform.matrix();
			// Set up vertex uniforms
			auto wvp = mShadowProj*worldMatrix;

			// render
			for(auto& primitive : renderObj->mesh->mPrimitives)
			{
				auto& command = mBackEnd.beginCommand();
				command.cullMode = affineTransformDeterminant(worldMatrix) < 0.f ? GL_BACK : GL_FRONT;
				mBackEnd.addParam(0, wvp);

				auto& mesh = *primitive.first;
				command.shader = getShader(mesh.vertexFormat());

				command.vao = mesh.getVao();
				command.nIndices = mesh.indices().count;
				command.indexType = mesh.indices().componentType;

				mBackEnd.endCommand();
			}
		}

		mBackEnd.endPass();
		mBackEnd.submitDraws();
	}

	//----------------------------------------------------------------------------------------------
	void ShadowMapPass::loadCommonShaderCode()
	{
		mCommonShaderCode = Shader::loadCodeFromFile("shadowMap.fx");
	}

	//----------------------------------------------------------------------------------------------
	Shader* ShadowMapPass::getShader(RenderGeom::VtxFormat vtxFormat)
	{
		auto pipelineCode = vtxFormat.code() & mVtxFormatMask; // Combine shader variations

		// Locate the proper shader in the set
		auto iter = mPipelines.find(pipelineCode);
		if(iter == mPipelines.end())
		{
			iter = mPipelines.emplace(
				pipelineCode,
				Shader::createShader({
					mCommonShaderCode.c_str()
					})
			).first;
		}
		return iter->second.get();
	}

}}