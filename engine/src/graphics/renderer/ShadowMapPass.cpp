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
#include <graphics/backend/device.h>
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
		, m_geomPass(device, ShaderCodeFragment::loadFromFile("shaders/shadowMap.fx"))
	{
		assert(target.isValid());
		// Renderpass
		RenderPass::Descriptor passDesc;
		passDesc.clearDepth = 1;
		passDesc.clearFlags = RenderPass::Descriptor::Clear::Depth;
		passDesc.target = target;
		passDesc.viewportSize = _size;
		m_pass = device.createRenderPass(passDesc);

		// Pipeline config
		m_rasterOptions.cullFront = true;
		m_rasterOptions.depthTest = Pipeline::DepthTest::Lequal;
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
		FrameBuffer::Attachment depthAttachment;
		depthAttachment.target = FrameBuffer::Attachment::Target::Depth;
		depthAttachment.imageType = FrameBuffer::Attachment::ImageType::Texture;
		depthAttachment.texture = texture;
		FrameBuffer::Descriptor shadowBufferDesc;
		shadowBufferDesc.numAttachments = 1;
		shadowBufferDesc.attachments = &depthAttachment;
		return device.createFrameBuffer(shadowBufferDesc);
	}

	//----------------------------------------------------------------------------------------------
	void ShadowMapPass::render(
		const std::vector<gfx::RenderItem>& shadowCasters,
		const std::vector<gfx::RenderItem>&,// shadowReceivers,
		const Camera& view,
		const Light& light,
		CommandBuffer& dst)
	{
		// Accumulate all casters into a single shadow space bounding box
		AABB castersBBox; castersBBox.clear();
		for(auto& obj : shadowCasters)
		{
			// Object's bounding box in shadow space
			auto bbox = obj.world * obj.geom.bbox();
			castersBBox.add(bbox);
		}

		auto world = light.worldMatrix; // So that light's +y axis (forward), maps to the -Z in camera
		adjustViewMatrix(world, castersBBox);// Adjust view matrix

		// Render
		dst.beginPass(*m_pass);
		renderMeshes(shadowCasters, dst); // Iterate over renderables
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
		biasMatrix(2,3) = -mBias;

		auto orthoSize = lightSpaceCastersBBox.size();
		auto castersMin = -orthoSize.z()/2;
		auto castersMax = orthoSize.z()/2;
		auto proj = math::orthographicMatrix(math::Vec2f(orthoSize.x(),orthoSize.y()), castersMin, castersMax);

		mShadowProj = proj * biasMatrix * shadowView.matrix();
		mUnbiasedShadowProj = proj * shadowView.matrix();
	}

	//----------------------------------------------------------------------------------------------
	void ShadowMapPass::renderMeshes(const std::vector<gfx::RenderItem>& renderables, CommandBuffer& dst)
	{
		auto worldMatrix = Mat44f::identity();

		GeometryPass::Instance instance;
		instance.geometryIndex = uint32_t(-1);
		instance.instanceCode = nullptr;

		// Retrieve all info needed to render the objects
		std::vector<GeometryPass::Instance> renderList;
		std::vector<const RenderGeom*> geometry;
		const RenderGeom* lastGeom = nullptr;

		for(auto& mesh : renderables)
		{
			// Raster options
			bool mirroredGeometry = affineTransformDeterminant(worldMatrix) < 0.f;
			m_rasterOptions.frontFace = mirroredGeometry ? Pipeline::Winding::CW : Pipeline::Winding::CCW;
			instance.raster = m_rasterOptions.mask();
			// Uniforms
			instance.uniforms.clear();
			Mat44f wvp = mShadowProj* mesh.world.matrix();
			instance.uniforms.mat4s.push_back({0, wvp});
			// Geometry
			if(lastGeom != &mesh.geom)
			{
				instance.geometryIndex++;
				geometry.push_back(&mesh.geom);
			}
			renderList.push_back(instance);
		}

		// Record commands
		m_geomPass.render(geometry, renderList, dst);
	}

}