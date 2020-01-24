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
	ShadowMapPass::ShadowMapPass(Device& device, const math::Vec2u& _size)
		: m_geomPass(device, ShaderCodeFragment::loadFromFile("shaders/shadowMap.fx"))
	{
		// Pipeline config
		m_rasterOptions.cullFront = true;
		m_rasterOptions.depthTest = Pipeline::DepthTest::Gequal;
	}

	//----------------------------------------------------------------------------------------------
	/*Texture2d ShadowMapPass::createShadowMapTexture(Device& device, const math::Vec2u& size)
	{
		auto shadowSamplerDesc = TextureSampler::Descriptor();
		shadowSamplerDesc.wrapS = TextureSampler::Wrap::Clamp;
		shadowSamplerDesc.wrapT = TextureSampler::Wrap::Clamp;
		shadowSamplerDesc.filter = TextureSampler::MinFilter::Linear;
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
		FrameBuffer::Descriptor shadowBufferDesc(1, &depthAttachment);
		return device.createFrameBuffer(shadowBufferDesc);
	}*/

	//----------------------------------------------------------------------------------------------
	void ShadowMapPass::render(
		const std::vector<gfx::RenderItem>& shadowCasters,
		const math::AABB& shadowReceiversViewSpaceBBox,
		const Camera& view,
		const Light& light,
		CommandBuffer& dst)
	{
		// Limit shadow draw distance
		Frustum vsShadowedVisibleVolume = Frustum(2.f, view.fov(), view.near(), mMaxShadowDistance);
		AABB viewSpaceAABB = vsShadowedVisibleVolume.boundingBox().intersection(shadowReceiversViewSpaceBBox);

		// Transform receiver volume into shadow space
		Mat44f shadowView = light.worldMatrix.orthoNormalInverse().matrix();
		AABB shadowSpaceRecVolume = (shadowView * view.world().matrix()) * viewSpaceAABB;
		// Expand the AABB to infinity in the shadow direction
		Vec3f shadowMax = shadowSpaceRecVolume.max();
		shadowMax.z() = std::numeric_limits<float>::max();
		shadowSpaceRecVolume.add(shadowMax);

		// Accumulate all casters into a single shadow space bounding box
		// Do this accumulation in view space to minimize the size of this bbox.
		// If we rotated the bbox first into world space, and then again into view space,
		// the result would be bigger.
		m_visibleCasters.clear();
		AABB castersBBox; castersBBox.clear(); // In view space
		for(auto& obj : shadowCasters)
		{
			// Object's bounding box in shadow space
			auto bbox = (shadowView*obj.world) * obj.geom->bbox();
			// We only care about casters that intersect the volume of visible receivers
			if(shadowSpaceRecVolume.intersect(bbox))
			{
				castersBBox.add(bbox);
				m_visibleCasters.push_back(obj);
			}
		}
		castersBBox = castersBBox.intersection(shadowSpaceRecVolume);

		// Re-center the view transform around the casters' AABB
		auto shadowWorld = light.worldMatrix;
		shadowView.col<3>() = shadowView.col<3>() - Vec4f(castersBBox.center(), 0.f);

		adjustViewMatrix(shadowView, castersBBox);// Adjust view matrix

		// Render
		//dst.beginPass(*m_pass);
		renderMeshes(m_visibleCasters, dst); // Iterate over renderables
	}

	//----------------------------------------------------------------------------------------------
	void ShadowMapPass::adjustViewMatrix(const math::Mat44f& shadowView, const math::AABB& castersBBox)
	{
		Mat44f biasMatrix = Mat44f::identity();
		biasMatrix(2,3) = -mBias;

		auto orthoSize = castersBBox.size();
		auto castersMin = -orthoSize.z()/2;
		auto castersMax = orthoSize.z()/2;
		auto proj = math::orthographicMatrix(orthoSize.xy(), castersMin, castersMax);

		mShadowProj = proj * biasMatrix * shadowView;
		mUnbiasedShadowProj = proj * shadowView;
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
			Mat44f wvp = mShadowProj* mesh.world;
			instance.uniforms.mat4s.push_back({0, wvp});
			// Geometry
			if(lastGeom != mesh.geom)
			{
				instance.geometryIndex++;
				geometry.push_back(mesh.geom);
			}
			renderList.push_back(instance);
		}

		// Record commands
		m_geomPass.render(geometry, renderList, dst);
	}

}