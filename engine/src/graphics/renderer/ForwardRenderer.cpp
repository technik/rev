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
#include <core/platform/fileSystem/file.h>
#include <core/platform/fileSystem/fileSystem.h>
#include <graphics/debug/debugGUI.h>
#include <graphics/debug/imgui.h>
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
		m_targetBuffer = target;

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
		shadowDesc.mipLevels = 1;
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

		// Background pass
		initBackgroundPass(device, targetSize);
	}

	//------------------------------------------------------------------------------------------------------------------
	void ForwardRenderer::render(const RenderScene& scene, const Camera& eye) {
		assert(m_targetBuffer.id != FrameBuffer::InvalidId);

		collapseSceneRenderables(scene);// Consolidate renderables into geometry (i.e. extracts geom from renderObj)

		// --- Cull stuff
		// Cull visible objects renderQ -> visible
		cull(m_renderQueue, m_visible, [&](const RenderItem& item) -> bool {
			return dot(item.world.position() - eye.position(), eye.viewDir()) > 0;
		});

		// TODO: Cull shadow casters renderQ -> casters
		// TODO: Cull visible shadow receivers visible -> receivers

		CommandBuffer::UniformBucket sharedUniforms;
		// Render shadows (casters, receivers)
		bool useShadows = !scene.lights().empty() && scene.lights()[0]->castShadows;
		if(useShadows)
		{
			mShadowPass->render(m_visible, m_visible, eye, *scene.lights()[0]);
			math::Mat44f shadowProj = mShadowPass->shadowProj();
			sharedUniforms.addParam(2, shadowProj);
			sharedUniforms.addParam(9, m_shadowsTexture);
			math::Vec3f lightDir = shadowProj.block<3,1>(0,2);
			sharedUniforms.addParam(6, lightDir);
		}

		// TODO: Sort visible objects
		auto env = &*scene.environment();
		mForwardPass->render(eye, env, useShadows, m_visible, sharedUniforms); // Render visible objects
		if(env && m_bgPass->isOk())
		{
			// Uniforms
			auto aspectRatio = float(m_targetSize.x())/m_targetSize.y();
			CommandBuffer::UniformBucket bgUniforms;
			bgUniforms.mat4s.push_back({0, eye.viewProj(aspectRatio) });
			bgUniforms.vec4s.push_back({1, math::Vec4f(float(m_targetSize.x()), float(m_targetSize.y()), 0.f, 0.f) });
			bgUniforms.floats.push_back({3, 0.f }); // Neutral exposure
			bgUniforms.textures.push_back({7, env->texture()} );
			// Render
			m_bgPass->render(bgUniforms);
			m_bgPass->submit();
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void ForwardRenderer::onResizeTarget(const math::Vec2u& _newSize)
	{
		// TODO: Resize shadow buffer accordingly, or at least the viewport it uses
		m_targetSize = _newSize;
		mForwardPass->onResizeTarget(_newSize);
		m_bgPass->onResizeTarget(_newSize);
	}

	//------------------------------------------------------------------------------------------------------------------
	void ForwardRenderer::drawDebugUI()
	{
		if(ImGui::Begin("Debug Fwd Renderer"))
		{
			ImGui::Image((void*)m_shadowsTexture.id, {256, 256});
		}
		ImGui::End();
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
	void ForwardRenderer::initBackgroundPass(gfx::Device& device, const math::Vec2u& targetSize)
	{
		assert(m_targetBuffer.valid());
		// Renderpass
		RenderPass::Descriptor passDesc;
		passDesc.clearFlags = RenderPass::Descriptor::Clear::None;
		passDesc.target = m_targetBuffer;
		passDesc.viewportSize = targetSize;
		m_bgPass = new FullScreenPass(device, passDesc);

		// Shader stages
		std::string skyShaderCode;
		ShaderProcessor::MetaData metadata;
		ShaderProcessor::loadCodeFromFile("sky.fx", skyShaderCode, metadata);
		m_bgPass->setPassCode(skyShaderCode.c_str());

		// Shader reload
		for(auto& file : metadata.dependencies)
		{
			core::FileSystem::get()->onFileChanged(file) += [this](const char*) {
				std::string skyShaderCode;
				ShaderProcessor::MetaData metadata;
				ShaderProcessor::loadCodeFromFile("sky.fx", skyShaderCode, metadata);
				m_bgPass->setPassCode(skyShaderCode.c_str());
			};
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