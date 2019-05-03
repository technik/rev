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

namespace rev::gfx {

	//------------------------------------------------------------------------------------------------------------------
	void ForwardRenderer::init(gfx::Device& device, const math::Vec2u& targetSize, gfx::FrameBuffer& target)
	{
		m_device = &device;
		m_targetBuffer = target;

		// Create the depth texture and framebuffer
		m_depthTexture = ZPrePass::createDepthMapTexture(device, targetSize);
		auto depthBuffer = ShadowMapPass::createShadowBuffer(device, m_depthTexture);
		mZPrePass = std::make_unique<ZPrePass>(device, depthBuffer, targetSize);
		
		// Create shadow pass
		unsigned shadowSize = 4*1024;
		auto shadowTexSize = math::Vec2u{shadowSize, shadowSize};
		m_shadowsTexture = ShadowMapPass::createShadowMapTexture(device, shadowTexSize);
		auto shadowBuffer = ShadowMapPass::createShadowBuffer(device, m_shadowsTexture);
		mShadowPass = std::make_unique<ShadowMapPass>(device, shadowBuffer, shadowTexSize);

		// Create the geometry forward pass
		mForwardPass = std::make_unique<ForwardPass>(device, targetSize, target);

		// Background pass
		initBackgroundPass(device, targetSize);
	}

	//------------------------------------------------------------------------------------------------------------------
	void ForwardRenderer::render(const RenderScene& scene, const Camera& eye) {
		assert(m_targetBuffer.isValid());

		collapseSceneRenderables(scene);// Consolidate renderables into geometry (i.e. extracts geom from renderObj)

		// --- Cull stuff
		// Cull visible objects renderQ -> visible
		cull(m_renderQueue, m_visible, [&](const RenderItem& item) -> bool {
			return dot(item.world.position() - eye.position(), eye.viewDir()) > 0;
		});

		// TODO: Cull shadow casters renderQ -> casters
		// TODO: Cull visible shadow receivers visible -> receivers

		// Z Prepass
		CommandBuffer frameCommands;
		mZPrePass->render(eye, m_visible, frameCommands);

		CommandBuffer::UniformBucket sharedUniforms;
		sharedUniforms.addParam(29, m_depthTexture);
		// Render shadows (casters, receivers)
		bool useShadows = !scene.lights().empty() && scene.lights()[0]->castShadows;
		if(useShadows)
		{
			mShadowPass->render(m_visible, m_visible, eye, *scene.lights()[0], frameCommands);
			math::Mat44f shadowProj = mShadowPass->shadowProj();
			sharedUniforms.addParam(2, shadowProj);
			sharedUniforms.addParam(9, m_shadowsTexture);
			math::Vec3f lightDir = shadowProj.block<3,1>(0,2);
			sharedUniforms.addParam(6, lightDir);
		}
		m_device->renderQueue().submitCommandBuffer(frameCommands);

		// Forward pass
		auto env = &*scene.environment();
		mForwardPass->render(eye, env, useShadows, m_visible, sharedUniforms, frameCommands); // Render visible objects

		// Background
		if(env)
		{
			// Uniforms
			auto aspectRatio = float(m_targetSize.x())/m_targetSize.y();
			CommandBuffer::UniformBucket bgUniforms;
			bgUniforms.mat4s.push_back({ 0, eye.world().matrix() });
			bgUniforms.mat4s.push_back({ 1, eye.projection(aspectRatio) });
			bgUniforms.vec4s.push_back({ 2, math::Vec4f(float(m_targetSize.x()), float(m_targetSize.y()), 0.f, 0.f) });
			bgUniforms.floats.push_back({3, 0.f }); // Neutral exposure
			bgUniforms.textures.push_back({7, env->texture()} );
			// Render
			m_bgPass->render(bgUniforms, frameCommands);
		}

		m_device->renderQueue().submitCommandBuffer(frameCommands);
	}

	//------------------------------------------------------------------------------------------------------------------
	void ForwardRenderer::onResizeTarget(const math::Vec2u& _newSize)
	{
		// TODO: Resize shadow buffer accordingly, or at least the viewport it uses
		m_targetSize = _newSize;
		mForwardPass->onResizeTarget(_newSize);
		if(m_depthTexture.isValid())
			m_device->destroyTexture2d(m_depthTexture);
		m_depthTexture = ZPrePass::createDepthMapTexture(*m_device, _newSize);
		mZPrePass->onResizeTarget(_newSize, m_depthTexture);
	}

	//------------------------------------------------------------------------------------------------------------------
	void ForwardRenderer::drawDebugUI()
	{
		if(ImGui::Begin("Debug Fwd Renderer"))
		{
			ImGui::Image((void*)m_shadowsTexture.id(), {256, 256});
		}
		ImGui::End();
	}

	//------------------------------------------------------------------------------------------------------------------
	void ForwardRenderer::collapseSceneRenderables(const RenderScene& scene)
	{
		m_visible.clear();
		for(auto obj : scene.renderables())
		{			
			if(!obj->visible)
				continue;
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
		assert(m_targetBuffer.isValid());
		// Renderpass
		RenderPass::Descriptor passDesc;
		passDesc.clearFlags = RenderPass::Descriptor::Clear::None;
		passDesc.target = m_targetBuffer;
		passDesc.viewportSize = targetSize;
		m_bgPass = std::make_unique<FullScreenPass>(device, ShaderCodeFragment::loadFromFile("shaders/sky.fx"));
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