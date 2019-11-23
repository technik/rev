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

#include "DeferredRenderer.h"
#include <graphics/backend/renderPass.h>
#include <graphics/renderGraph/frameBufferCache.h>
#include <graphics/scene/camera.h>
#include <graphics/scene/renderGeom.h>
#include <graphics/scene/renderMesh.h>
#include <graphics/scene/renderObj.h>
#include <graphics/scene/renderScene.h>
#include <graphics/shaders/shaderCodeFragment.h>
#include <math/algebra/vector.h>
#include <math/algebra/matrix.h>

using namespace rev::math;

namespace rev::gfx {

	//----------------------------------------------------------------------------------------------
	void DeferredRenderer::init(Device& device, const math::Vec2u& size, FrameBuffer target)
	{
		m_targetFb = target;
		m_device = &device;
		m_fbCache = std::make_unique<FrameBufferCache>(device);
		m_viewportSize = size;
		const unsigned shadowBufferSize = 1024;
		m_shadowSize = Vec2u(shadowBufferSize, shadowBufferSize);
		createRenderPasses(target);

		// Load ibl texture
		auto iblImg = Image::load("shaders/ibl_brdf.hdr", 4);
		TextureSampler::Descriptor samplerDesc;
		samplerDesc.filter = TextureSampler::MinFilter::Linear;
		samplerDesc.wrapS = TextureSampler::Wrap::Clamp;
		samplerDesc.wrapT = TextureSampler::Wrap::Clamp;

		Texture2d::Descriptor ibl_desc;
		ibl_desc.mipLevels = 1;
		ibl_desc.sRGB = false;
		ibl_desc.pixelFormat = iblImg->format();
		ibl_desc.size = iblImg->size();
		ibl_desc.sampler = device.createTextureSampler(samplerDesc);
		ibl_desc.srcImages.emplace_back(std::move(iblImg));

		m_brdfIbl = device.createTexture2d(ibl_desc);
	}

	//----------------------------------------------------------------------------------------------
	void DeferredRenderer::render(const RenderScene& scene, const Camera& eye)
	{
		// TODO: maybe move the actual ownership of the framegraph to whoever calls the renderer.
		// That way, keeping the graph alive is the caller´s decision, and so graphcs can be cached, etc.
		RenderGraph frameGraph(*m_device);

		// --- Cull stuff
		collapseSceneRenderables(scene);// Consolidate renderables into geometry (i.e. extracts geom from renderObj)
		// Cull visible objects renderQ -> visible
		m_opaqueQueue.clear();
		m_transparentQueue.clear();
		Mat44f view = eye.view();
		for (auto& renderItem : m_renderQueue)
		{
			AABB viewSpaceBB = (view * renderItem.world) * renderItem.geom.bbox();
			if (viewSpaceBB.min().z() < 0)
			{
				if (renderItem.material.alpha() == Material::Alpha::opaque)
					m_opaqueQueue.push_back(renderItem);
				else
					m_transparentQueue.push_back(renderItem);

			}
		}

		float aspectRatio = float(m_viewportSize.x()) / m_viewportSize.y();
		auto viewProj = eye.viewProj(aspectRatio);

		// G-Buffer pass
		RenderGraph::BufferResource depth, normals, pbr, albedo; // G-Pass outputs
		frameGraph.addPass("G-Buffer",
			m_viewportSize,
			// Pass definition
			[&](RenderGraph::IPassBuilder& pass) {
				normals = pass.write(BufferFormat::RGBA8);
				albedo = pass.write(BufferFormat::sRGBA8);
				pbr = pass.write(BufferFormat::RGBA8);
				depth = pass.write(BufferFormat::depth32);
			},
			// Pass evaluation
			[&](const Texture2d* inputTextures, size_t nInputTextures, CommandBuffer& dst)
			{
				// Clear depth
				dst.clearDepth(0.f);
				dst.clearColor(Vec4f::zero());
				dst.clear(Clear::All);

				m_gPass->render(viewProj, m_opaqueQueue, m_rasterOptions, dst);
			});

		// Optional shadow pass
		const bool useShadows = !scene.lights().empty() && scene.lights()[0]->castShadows;
		RenderGraph::BufferResource shadows;
		if (useShadows)
		{
			frameGraph.addPass("Sky shadow",
				m_shadowSize,
				[&](RenderGraph::IPassBuilder& pass)
				{
					shadows = pass.write(BufferFormat::depth32);
				},
				[&](const Texture2d*, size_t, CommandBuffer& dst)
				{
					dst.clearDepth(0.f);
					dst.clear(Clear::Depth);
					auto& light = *scene.lights()[0];
					m_shadowPass->render(m_renderQueue, m_opaqueQueue, eye, light, dst);
				});
		}

		// Environment light pass
		//RenderGraph::BufferResource hdr;
		frameGraph.addPass("Light pass",
			m_viewportSize,
			// Pass definition
			[&](RenderGraph::IPassBuilder& pass) {
			//hdr = pass.write(BufferFormat::RGBA32);
				pass.write(m_targetFb); // Should write to hdr

				pass.read(normals, 0);
				pass.read(albedo, 1);
				pass.read(pbr, 2);
				pass.read(depth, 3);
				// TODO: Shadows enabled? read them!
				if (useShadows)
					pass.read(shadows, 4);
			},
			// Pass evaluation
			[&](const Texture2d* inputTextures, size_t nInputTextures, CommandBuffer& dst)
			{
				dst.clearColor(Vec4f::zero());
				dst.clearDepth(0.f);
				dst.clear(Clear::All);

				// Light-pass
				if (auto env = scene.environment())
				{
					CommandBuffer::UniformBucket envUniforms;
					auto projection = eye.projection(aspectRatio);
					Mat44f invView = eye.world().matrix();
					envUniforms.addParam(0, projection);
					envUniforms.addParam(1, invView);
					envUniforms.addParam(2, math::Vec4f(float(m_viewportSize.x()), float(m_viewportSize.y()), 0.f, 0.f));
					envUniforms.addParam(3, 1.f);

					envUniforms.addParam(4, env->texture());
					envUniforms.addParam(5, m_brdfIbl);
					envUniforms.addParam(6, (float)env->numLevels());

					envUniforms.addParam(7, inputTextures[0]);
					envUniforms.addParam(8, inputTextures[3]);
					envUniforms.addParam(9, inputTextures[2]);
					envUniforms.addParam(10, inputTextures[1]);

					if (useShadows)
					{
						math::Mat44f shadowProj = m_shadowPass->shadowProj();
						envUniforms.addParam(11, inputTextures[4]);
						envUniforms.addParam(12, shadowProj);
					}

					m_lightingPass->render(envUniforms, dst);

					// Background
					// Uniforms
					CommandBuffer::UniformBucket bgUniforms;
					bgUniforms.mat4s.push_back({ 0, invView });
					bgUniforms.mat4s.push_back({ 1, projection });
					bgUniforms.vec4s.push_back({ 2, math::Vec4f(float(m_viewportSize.x()), float(m_viewportSize.y()), 0.f, 0.f) });
					bgUniforms.floats.push_back({ 3, 0.f }); // Neutral exposure
					bgUniforms.textures.push_back({ 7, env->texture() });
					// Render
					m_bgPass->render(bgUniforms, dst);
				}
			});

		// Record passes
		frameGraph.build(*m_fbCache);
		CommandBuffer frameCommands;
		frameGraph.evaluate(frameCommands);

		// Submit
		m_device->renderQueue().submitCommandBuffer(frameCommands);
	}

	//----------------------------------------------------------------------------------------------
	void DeferredRenderer::onResizeTarget(const math::Vec2u& _newSize)
	{
		// Skip it for now
		m_fbCache->deallocateResources();
		m_viewportSize = _newSize;
		// Recreate internal buffers
		if(m_gPass)
			delete m_gPass;
		if(m_lightingPass)
			delete m_lightingPass;
		createRenderPasses(m_targetFb);
	}

	//----------------------------------------------------------------------------------------------
	void DeferredRenderer::createRenderPasses(gfx::FrameBuffer target)
	{
		m_rasterOptions.cullBack = true;
		m_rasterOptions.depthTest = Pipeline::DepthTest::Gequal;

		m_gPass = new GeometryPass(*m_device, ShaderCodeFragment::loadFromFile("shaders/gbuffer.fx"));

		// Shadow pass
		m_shadowPass = std::make_unique<ShadowMapPass>(*m_device, m_shadowSize);

		// Lighting pass
		m_lightingPass = new FullScreenPass(*m_device, ShaderCodeFragment::loadFromFile("shaders/lightPass.fx"));

		// Background pass
		m_bgPass = std::make_unique<FullScreenPass>(*m_device, ShaderCodeFragment::loadFromFile("shaders/sky.fx"));
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class Filter> // Filter must be an operator (RenderItem) -> bool
	void DeferredRenderer::cull(const std::vector<RenderItem>& from, std::vector<RenderItem>& to, const Filter& filter) // TODO: Cull inplace?
	{
		for(auto& item : from)
			if(filter(item))
				to.push_back(item);
	}

	//------------------------------------------------------------------------------------------------------------------
	void DeferredRenderer::collapseSceneRenderables(const RenderScene& scene)
	{
		m_renderQueue.clear();
		for(auto obj : scene.renderables())
		{			
			if(!obj->visible)
				continue;
			for(auto mesh : obj->mesh->mPrimitives)
			{
				assert(mesh.first && mesh.second);
				m_renderQueue.push_back({obj->transform, *mesh.first, *mesh.second});
			}
		}
	}

} // namespace rev::gfx