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
#include <graphics/backend/commandBuffer.h>
#include <graphics/backend/renderPass.h>
#include <graphics/debug/imgui.h>
#include <graphics/driver/shaderProcessor.h>
#include <graphics/scene/camera.h>
#include <graphics/scene/renderGeom.h>
#include <graphics/scene/renderMesh.h>
#include <graphics/scene/renderObj.h>
#include <graphics/scene/renderScene.h>
#include <math/algebra/affineTransform.h>
#include <string>

using namespace rev::math;
using namespace rev::gfx;
using namespace std;

namespace rev { namespace graphics {

	//----------------------------------------------------------------------------------------------
	ShadowMapPass::ShadowMapPass(gfx::Device& device, gfx::FrameBuffer target, const math::Vec2u& _size)
		: m_device(device)
	{
		// Renderpass
		RenderPass::Descriptor passDesc;
		passDesc.clearDepth = 0;
		passDesc.clearFlags = RenderPass::Descriptor::Clear::Depth;
		passDesc.target = target;
		passDesc.viewportSize = _size;
		m_pass = device.createRenderPass(passDesc);

		// Shader stages
		Pipeline::ShaderModule::Descriptor stageDesc;
		loadCommonShaderCode();
		stageDesc.code = { mCommonShaderCode };
		stageDesc.stage = Pipeline::ShaderModule::Descriptor::Vertex;
		m_commonPipelineDesc.vtxShader = m_device.createShaderModule(stageDesc);
		stageDesc.stage = Pipeline::ShaderModule::Descriptor::Pixel;
		m_commonPipelineDesc.pxlShader = m_device.createShaderModule(stageDesc);
		// Pipeline config
		m_commonPipelineDesc.cullFront = true;
		m_commonPipelineDesc.depthTest = Pipeline::Descriptor::DepthTest::Lequal;

		// Vertex format
		mVtxFormatMask = RenderGeom::VtxFormat(
			RenderGeom::VtxFormat::Storage::Float32,
			RenderGeom::VtxFormat::Storage::None,
			RenderGeom::VtxFormat::Storage::None,
			RenderGeom::VtxFormat::Storage::None,
			RenderGeom::VtxFormat::Storage::None
		).code(); // We only care about the format of vertex positions

	}

	//----------------------------------------------------------------------------------------------
	void ShadowMapPass::render(const std::vector<std::shared_ptr<RenderObj>>& renderables, const Camera& viewCam, const Light& light)
	{
		// Accumulate all casters into a single shadow space bounding box
		AABB castersBBox; castersBBox.clear();
		AffineTransform lightRotation = AffineTransform::identity();
		lightRotation.setRotation(Quatf({1.f, 0.f, 0.f}, HalfPi));
		auto world = light.worldMatrix * lightRotation; // So that light's +y axis (forward), maps to the -Z in camera
		auto view = world.orthoNormalInverse();
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

		adjustViewMatrix(world, castersBBox);// Adjust view matrix

		// Render
		renderMeshes(renderables); // Iterate over renderables
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
		auto castersMin = -orthoSize.z()/2;
		auto castersMax = orthoSize.z()/2;
		auto proj = math::orthographicMatrix(math::Vec2f(orthoSize.x(),orthoSize.y()), castersMin, castersMax);

		mShadowProj = proj * biasMatrix * shadowView.matrix();
		mUnbiasedShadowProj = proj * shadowView.matrix();
	}

	//----------------------------------------------------------------------------------------------
	void ShadowMapPass::renderMeshes(const std::vector<std::shared_ptr<RenderObj>>& renderables)
	{
		auto worldMatrix = Mat44f::identity();

		struct RenderInfo
		{
			Pipeline pipeline;
			Mat44f wvp;
			RenderGeom* renderable;
		};

		// Retrieve all info needed to render the objects
		std::vector<RenderInfo> renderList;

		for(auto& renderable : renderables)
		{
			RenderInfo info;
			worldMatrix.block<3,4>(0,0) = renderable->transform.matrix();
			info.wvp = mShadowProj*worldMatrix;

			bool mirroredGeometry = affineTransformDeterminant(worldMatrix) < 0.f;
			for(auto& primitive : renderable->mesh->mPrimitives)
			{
				auto& mesh = *primitive.first;
				info.pipeline = getPipeline(mesh.vertexFormat(), mirroredGeometry);
				info.renderable = &mesh;
				renderList.push_back(info);
			}
		}

		// Sort renderables to reuse pipelines as much as possible
		std::sort(renderList.begin(), renderList.end(), [](auto& a, auto& b){
			return a.pipeline.id < b.pipeline.id;
		});

		// Record commands
		CommandBuffer cmdBuffer;
		Pipeline activePipeline; // Invalid id

		CommandBuffer::UniformBucket uniforms;

		for(auto& draw : renderList)
		{
			// Pipeline
			if(draw.pipeline.id != activePipeline.id)
			{
				cmdBuffer.setPipeline(draw.pipeline);
				activePipeline = draw.pipeline;
			}
			// Uniform
			uniforms.mat4s.push_back({0, draw.wvp});
			cmdBuffer.setUniformData(uniforms);
			uniforms.clear();
			// Geometry
			cmdBuffer.setVertexData(draw.renderable->getVao());
			assert(draw.renderable->indices().componentType == GL_UNSIGNED_SHORT);
			cmdBuffer.drawTriangles(draw.renderable->indices().count, CommandBuffer::IndexType::U16); // TODO, this isn't always U16
		}

		m_pass->reset();
		m_pass->record(cmdBuffer);
		m_device.renderQueue().submitPass(*m_pass);
	}

	//----------------------------------------------------------------------------------------------
	void ShadowMapPass::loadCommonShaderCode()
	{
		ShaderProcessor::MetaData metadata;
		ShaderProcessor::loadCodeFromFile("shadowMap.fx", mCommonShaderCode, metadata);
		// TODO: Actually use the metadata (unifrom layouts)
	}

	//----------------------------------------------------------------------------------------------
	Pipeline ShadowMapPass::getPipeline(RenderGeom::VtxFormat vtxFormat, bool mirroredGeometry)
	{		
		auto& pipelineMap = mirroredGeometry ? m_mirroredPipelines : m_regularPipelines;

		auto pipelineCode = vtxFormat.code() & mVtxFormatMask; // Combine shader variations

		auto iter = pipelineMap.find(pipelineCode);
		if(iter == pipelineMap.end())
		{
			auto pipelineDesc = m_commonPipelineDesc;
			pipelineDesc.frontFace = mirroredGeometry ? Pipeline::Descriptor::CW : Pipeline::Descriptor::CCW;
			
			iter = pipelineMap.emplace(
				pipelineCode,
				m_device.createPipeline(pipelineDesc)
			).first;
		}
		return mirroredGeometry ? m_mirroredPipelines[pipelineCode] : m_regularPipelines[pipelineCode];
	}

}}