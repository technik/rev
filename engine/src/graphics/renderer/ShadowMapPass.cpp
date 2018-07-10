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
#include <graphics/driver/shaderProcessor.h>
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
	void ShadowMapPass::render(const std::vector<std::shared_ptr<RenderObj>>& renderables, const Camera& viewCam, const Light& light)
	{
		// Accumulate all casters into a single shadow space bounding box
		AABB castersBBox; castersBBox.clear();
		auto view = light.worldMatrix.orthoNormalInverse();
		for(auto& obj : renderables)
		{
			auto modelToShadow = view * obj->transform;
			for(auto& primitive : obj->mesh->mPrimitives)
			{
				// Object's bounding box in shadow space
				auto bbox = primitive.first->bbox().transform(modelToShadow);
				castersBBox = math::AABB(castersBBox, bbox);
			}
		}

		adjustViewMatrix(light.worldMatrix,castersBBox);// Adjust view matrix

		// Render
		setUpGlobalState(); // Set gl global state
		renderMeshes(renderables); // Iterate over renderables
	}

	//----------------------------------------------------------------------------------------------
	void ShadowMapPass::setUpGlobalState()
	{
		mDepthBuffer->bind();
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_GEQUAL); // Keeping the closest back face reduces precision problems

		glClearDepthf(0.0);
		glClear(GL_DEPTH_BUFFER_BIT);

		glEnable(GL_CULL_FACE);
		//glDisable(GL_CULL_FACE);
	}

	//----------------------------------------------------------------------------------------------
	void ShadowMapPass::adjustViewMatrix(const math::AffineTransform& shadowWorld, const math::AABB& castersBBox)
	{
		auto shadowWorldXForm = shadowWorld;
		auto shadowCenter = shadowWorld.transformPosition(castersBBox.origin());
		shadowWorldXForm.position() = shadowCenter;
		auto shadowView = shadowWorldXForm.orthoNormalInverse();

		if(ImGui::Begin("ShadowMap"))
		{
			ImGui::SliderFloat("Shadow bias", &mBias, -0.01f, 0.01f);
		}
		ImGui::End();

		Mat44f biasMatrix = Mat44f::identity();
		biasMatrix(1,3) = -mBias;

		auto orthoSize = castersBBox.size();
		auto castersMin = -orthoSize.y()/2;
		auto castersMax = orthoSize.y()/2;
		auto proj = math::orthographicMatrix(math::Vec2f(orthoSize.x(),orthoSize.z()), castersMin, castersMax);

		mShadowProj = proj * biasMatrix * shadowView.matrix();
		mUnbiasedShadowProj = proj * shadowView.matrix();
	}

	//----------------------------------------------------------------------------------------------
	void ShadowMapPass::renderMeshes(const std::vector<std::shared_ptr<RenderObj>>& renderables)
	{
		auto worldMatrix = Mat44f::identity();
		// TODO: Performance counters
		mBackEnd.beginPass();

		for(auto& renderable : renderables)
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
				command.cullMode = affineTransformDeterminant(worldMatrix) > 0.f ? GL_FRONT : GL_BACK;
				//command.cullMode = GL_FRONT;
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
		ShaderProcessor::MetaData metadata;
		ShaderProcessor::loadCodeFromFile("shadowMap.fx", mCommonShaderCode, metadata);
		// TODO: Actualle use the metadata (unifrom layouts)
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