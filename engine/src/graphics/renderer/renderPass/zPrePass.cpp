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
#include "zPrePass.h"

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
	ZPrePass::ZPrePass(gfx::Device& device, gfx::FrameBuffer target, const math::Vec2u& _size)
		: m_device(device)
		, m_geomPass(device, m_commonCode)
	{
		assert(target.valid());
		m_viewportSize = _size;

		// Renderpass
		m_frameBuffer = target;
		createRenderPass(_size);

		// Pipeline config
		m_commonCode = ShaderCodeFragment::loadFromFile("../data/shaders/zPrePass.fx");
		m_rasterOptions.cullBack = true;
		m_rasterOptions.depthTest = Pipeline::DepthTest::Lequal;
	}

	//----------------------------------------------------------------------------------------------
	void ZPrePass::createRenderPass(const math::Vec2u& _size)
	{
		RenderPass::Descriptor passDesc;
		passDesc.clearDepth = 1;
		passDesc.clearFlags = RenderPass::Descriptor::Clear::Depth;
		passDesc.target = m_frameBuffer;
		passDesc.viewportSize = _size;
		m_pass = m_device.createRenderPass(passDesc);
		m_pass->record(m_commands);
	}

	//----------------------------------------------------------------------------------------------
	Texture2d ZPrePass::createDepthMapTexture(Device& device, const math::Vec2u& size)
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
	FrameBuffer ZPrePass::createDepthBuffer(Device& device, Texture2d texture)
	{
		gfx::FrameBuffer::Attachment depthAttachment;
		depthAttachment.target = gfx::FrameBuffer::Attachment::Target::Depth;
		depthAttachment.texture = texture;
		gfx::FrameBuffer::Descriptor shadowBufferDesc;
		shadowBufferDesc.numAttachments = 1;
		shadowBufferDesc.attachments = &depthAttachment;
		return device.createFrameBuffer(shadowBufferDesc);
	}

	void ZPrePass::onResizeTarget(const math::Vec2u& newSize, Texture2d targetTexture) {
		m_viewportSize = newSize;
		m_device.destroyRenderPass(*m_pass);
		m_frameBuffer = createDepthBuffer(m_device, targetTexture);
		createRenderPass(newSize);
	}

	//----------------------------------------------------------------------------------------------
	void ZPrePass::render(
		const Camera& eye,
		const std::vector<gfx::RenderItem>& renderables
	)
	{
		auto worldMatrix = Mat44f::identity();

		GeometryPass::Instance instance;
		instance.geometryIndex = uint32_t(-1);
		instance.instanceCode = nullptr;

		// Retrieve all info needed to render the objects
		std::vector<GeometryPass::Instance> renderList;
		std::vector<const RenderGeom*> geometry;
		const RenderGeom* lastGeom = nullptr;

		float aspectRatio = float(m_viewportSize.x())/m_viewportSize.y();
		auto viewProj = eye.viewProj(aspectRatio);

		for(auto& mesh : renderables)
		{
			// Raster options
			bool mirroredGeometry = affineTransformDeterminant(worldMatrix) < 0.f;
			m_rasterOptions.frontFace = mirroredGeometry ? Pipeline::Winding::CW : Pipeline::Winding::CCW;
			instance.raster = m_rasterOptions.mask();
			// Uniforms
			instance.uniforms.clear();
			Mat44f wvp = viewProj * mesh.world.matrix();
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
		m_commands.clear();
		m_geomPass.render(geometry, renderList, m_commands);
	}

	//----------------------------------------------------------------------------------------------
	void ZPrePass::submit()
	{
		m_device.renderQueue().submitPass(*m_pass);
	}
}