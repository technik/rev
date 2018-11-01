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

namespace rev::gfx {

	//----------------------------------------------------------------------------------------------
	ShadowMapPass::ShadowMapPass(gfx::Device& device, gfx::FrameBuffer target, const math::Vec2u& _size)
		: m_device(device)
	{
		assert(target.valid());
		// Renderpass
		RenderPass::Descriptor passDesc;
		passDesc.clearDepth = 1;
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
	Texture2d ShadowMapPass::createShadowMapTexture(Device& device, const math::Vec2u& size)
	{
		auto shadowSamplerDesc = TextureSampler::Descriptor();
		shadowSamplerDesc.wrapS = TextureSampler::Descriptor::Wrap::Clamp;
		shadowSamplerDesc.wrapT = TextureSampler::Descriptor::Wrap::Clamp;
		shadowSamplerDesc.filter = TextureSampler::Descriptor::MinFilter::Linear;
		auto shadowSampler = device.createTextureSampler(shadowSamplerDesc);
		auto shadowDesc = Texture2d::Descriptor();
		shadowDesc.pixelFormat.channel = Image::ChannelFormat::Float32;
		shadowDesc.pixelFormat.numChannels = 1;
		shadowDesc.depth = true;
		shadowDesc.sampler = shadowSampler;
		shadowDesc.mipLevels = 1;
		shadowDesc.size = size;

		return device.createTexture2d(shadowDesc);
	}

	//----------------------------------------------------------------------------------------------
	FrameBuffer ShadowMapPass::createShadowBuffer(Device& device, Texture2d texture)
	{
		gfx::FrameBuffer::Attachment depthAttachment;
		depthAttachment.target = gfx::FrameBuffer::Attachment::Target::Depth;
		depthAttachment.texture = texture;
		gfx::FrameBuffer::Descriptor shadowBufferDesc;
		shadowBufferDesc.numAttachments = 1;
		shadowBufferDesc.attachments = &depthAttachment;
		return device.createFrameBuffer(shadowBufferDesc);
	}

	//----------------------------------------------------------------------------------------------
	void ShadowMapPass::render(
		const std::vector<gfx::RenderItem>& shadowCasters,
		const std::vector<gfx::RenderItem>&,// shadowReceivers,
		const Camera& view,
		const Light& light)
	{
		// Accumulate all casters into a single shadow space bounding box
		AABB castersBBox; castersBBox.clear();
		for(auto& obj : shadowCasters)
		{
			// Object's bounding box in shadow space
			auto bbox = obj.geom.bbox().transform(obj.world);
			castersBBox = math::AABB(castersBBox, bbox);
		}

		auto world = light.worldMatrix; // So that light's +y axis (forward), maps to the -Z in camera
		adjustViewMatrix(world, castersBBox);// Adjust view matrix

		// Render
		renderMeshes(shadowCasters); // Iterate over renderables
	}

	//----------------------------------------------------------------------------------------------
	void ShadowMapPass::adjustViewMatrix(const math::AffineTransform& wsLight, const math::AABB& castersBBox)
	{
		auto shadowWorldXForm = wsLight;
		auto shadowCenter = castersBBox.origin();
		shadowWorldXForm.position() = shadowCenter;
		auto shadowView = shadowWorldXForm.orthoNormalInverse();

		auto lightSpaceCastersBBox = castersBBox.transform(shadowWorldXForm);
		Mat44f biasMatrix = Mat44f::identity();
		biasMatrix(2,3) = mBias;

		auto orthoSize = lightSpaceCastersBBox.size();
		auto castersMin = -orthoSize.z()/2;
		auto castersMax = orthoSize.z()/2;
		auto proj = math::orthographicMatrix(math::Vec2f(orthoSize.x(),orthoSize.y()), castersMin, castersMax);

		mShadowProj = proj * biasMatrix * shadowView.matrix();
		mUnbiasedShadowProj = proj * shadowView.matrix();
	}

	//----------------------------------------------------------------------------------------------
	void ShadowMapPass::renderMeshes(const std::vector<gfx::RenderItem>& renderables)
	{
		auto worldMatrix = Mat44f::identity();

		struct RenderInfo
		{
			Pipeline pipeline;
			Mat44f wvp;
			const RenderGeom* renderable;
		};

		// Retrieve all info needed to render the objects
		std::vector<RenderInfo> renderList;

		for(auto& mesh : renderables)
		{
			Mat44f wvp = mShadowProj* mesh.world.matrix();
			bool mirroredGeometry = affineTransformDeterminant(worldMatrix) < 0.f;
			auto pipeline = getPipeline(mesh.geom.vertexFormat(), mirroredGeometry);

			renderList.push_back({
				pipeline,
				wvp,
				&mesh.geom
			});
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

}