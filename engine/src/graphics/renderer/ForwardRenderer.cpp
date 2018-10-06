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
#include "ForwardRenderer.h"
#include "ForwardPass.h"
#include "graphics/driver/renderTarget.h"
#include "graphics/scene/renderScene.h"
#include <graphics/scene/renderObj.h>
#include <graphics/scene/renderGeom.h>
#include <graphics/scene/renderMesh.h>
#include <graphics/backend/device.h>
#include <graphics/backend/texture2d.h>
#include <graphics/backend/textureSampler.h>
#include <math/algebra/vector.h>

using namespace rev::gfx;

namespace rev::gfx {

	//------------------------------------------------------------------------------------------------------------------
	void ForwardRenderer::init(gfx::Device& device, const math::Vec2u& targetSize, gfx::FrameBuffer& target)
	{
		m_targetBuffer = &target;

		mForwardPass = std::make_unique<ForwardPass>(device, targetSize, target);

		// Create a depth texture for shadows
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
		size_t shadowSize = 4*1024;
		shadowDesc.size = {shadowSize, shadowSize};
		m_shadowsTexture = device.createTexture2d(shadowDesc);

		// Create the depth framebuffer
		gfx::FrameBuffer::Attachment depthAttachment;
		depthAttachment.target = gfx::FrameBuffer::Attachment::Target::Depth;
		depthAttachment.texture = m_shadowsTexture;
		gfx::FrameBuffer::Descriptor shadowBufferDesc;
		shadowBufferDesc.numAttachments = 1;
		shadowBufferDesc.attachments = &depthAttachment;
		auto shadowBuffer = device.createFrameBuffer(shadowBufferDesc);

		mShadowPass = std::make_unique<ShadowMapPass>(device, shadowBuffer, shadowDesc.size);

		//mSkyPlane = std::make_unique<RenderGeom>(RenderGeom::quad(2.f*Vec2f::ones()));
	}

	//------------------------------------------------------------------------------------------------------------------
	void ForwardRenderer::render(const RenderScene& scene, const Camera& eye) {
		assert(m_targetBuffer);

		collapseSceneRenderables(scene);// Consolidate renderables into geometry (i.e. extracts geom from renderObj)

		// --- Cull stuff
		// Cull visible objects renderQ -> visible
		cull(m_renderQueue, m_visible, [&](const RenderItem& item) -> bool {
			return dot(item.world.position() - eye.position(), eye.viewDir()) > 0;
		});

		// TODO: Cull shadow casters renderQ -> casters
		// TODO: Cull visible shadow receivers visible -> receivers

		// Render shadows (casters, receivers)
		if(!scene.lights().empty() && scene.lights()[0]->castShadows)
		{
			mShadowPass->render(m_renderQueue, m_visible, eye, *scene.lights()[0]);
		}

		// TODO: Sort visible objects
		mForwardPass->render(eye, m_visible, m_shadowsTexture); // Render visible objects
		// TODO: Render background
	}

	//------------------------------------------------------------------------------------------------------------------
	void ForwardRenderer::onResizeTarget(const math::Vec2u& _newSize)
	{
		// TODO: Resize shadow buffer accordingly, or at least the viewport it uses
		mForwardPass->onResizeTarget(_newSize);
	}

	//------------------------------------------------------------------------------------------------------------------
	void ForwardRenderer::collapseSceneRenderables(const RenderScene& scene)
	{
		m_visible.clear();
		for(auto obj : scene.renderables())
		{			
			for(auto mesh : obj->mesh->mPrimitives)
			{
				assert(mesh.first && mesh.second);
				m_visible.push_back({obj->transform, *mesh.first, *mesh.second});
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class Filter> // Filter must be an operator (RenderItem) -> bool
	void ForwardRenderer::cull(const std::vector<RenderItem>& from, std::vector<RenderItem>& to, const Filter& filter) // TODO: Cull inplace?
	{
		for(auto& item : from)
			if(filter(item))
				to.push_back(item);
	}

}