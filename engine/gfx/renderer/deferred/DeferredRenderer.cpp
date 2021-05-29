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

/*#include "DeferredRenderer.h"
#include <gfx/renderGraph/frameBufferCache.h>
#include <gfx/scene/camera.h>
#include <gfx/scene/renderGeom.h>
#include <gfx/scene/renderMesh.h>
#include <gfx/scene/renderObj.h>
#include <gfx/scene/renderScene.h>
#include <gfx/shaders/shaderCodeFragment.h>
#include <math/algebra/vector.h>
#include <math/algebra/matrix.h>

#include <sstream>

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

		// Blue noise
		m_noisePermutations = std::uniform_int_distribution<unsigned>(0, NumBlueNoiseTextures - 1);
		loadNoiseTextures();
	}

	//----------------------------------------------------------------------------------------------
	void DeferredRenderer::render(const RenderScene& scene, const Camera& eye)
	{
		// TODO: maybe move the actual ownership of the framegraph to whoever calls the renderer.
		// That way, keeping the graph alive is the caller�s decision, and so graphcs can be cached, etc.
		RenderGraph frameGraph(*m_device);

		ImGui::Begin("Deferred renderer");

		// --- Cull visible renderables
		ImGui::Text("Renderables: %d", scene.renderables().size());
		float aspectRatio = float(m_viewportSize.x()) / m_viewportSize.y();

		// Culling config must happen before actual culling in collapseSceneRenderables
		ImGui::Checkbox("Lock culling", &m_lockCulling);
		Mat44f view = eye.view();
		if (!m_lockCulling)
		{
			m_cullingFrustum = eye.frustum(aspectRatio);
			m_cullingViewMtx = view;
		}

		// Cull visible objects renderQ -> visible
		collapseSceneRenderables(scene, eye);// Consolidate renderables into geometry (i.e. extracts geom from renderObj)
		ImGui::Text("Visible: %d", m_visibleQueue.size());
		sortVisibleQueue();

		// Classify visible objects into separate render queues
		m_opaqueQueue.clear();
		m_alphaMaskQueue.clear();
		m_emissiveMaskQueue.clear();
		m_transparentQueue.clear();
		m_emissiveQueue.clear();
		
		for (auto& renderItem : m_visibleQueue)
		{
			AABB viewSpaceBB = (view * renderItem.world) * renderItem.geom->bbox();
			if (viewSpaceBB.min().z() < 0)
			{
				switch (renderItem.material->transparency())
				{
					case Material::Transparency::Opaque:
					{
						if (renderItem.material->isEmissive())
							m_emissiveQueue.push_back(renderItem);
						else
							m_opaqueQueue.push_back(renderItem);
						break;
					}
					case Material::Transparency::Mask:
					{
						if (renderItem.material->isEmissive())
							m_emissiveMaskQueue.push_back(renderItem);
						else
							m_alphaMaskQueue.push_back(renderItem);
						break;
					}
					case Material::Transparency::Blend:
					{
						m_transparentQueue.push_back(renderItem);
						break;
					}
				}
			}
		}

		bool useEmissive = !m_emissiveQueue.empty();
		auto viewMtx = eye.view();
		auto projMtx = eye.projection(aspectRatio);

		// G-Buffer pass with emissive
		RenderGraph::BufferResource depth, normals, pbr, albedo, hdr; // G-Pass outputs
		if (useEmissive)
		{
			frameGraph.addPass("Emissive clear",
				m_viewportSize,
				// Pass definition
				[&](RenderGraph::IPassBuilder& pass) {
					hdr = pass.write(BufferFormat::RGBA32);
				},
				// Pass evaluation
				[&](const Texture2d* inputTextures, size_t nInputTextures, CommandBuffer& dst)
				{
					// Clear depth
					dst.clearDepth(0.f);
					dst.clearColor(Vec4f(0.f,0.f,0.f,1.f));
					dst.clear(Clear::All);
				});
		}

		// G-Buffer pass
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
				dst.clearColor(Vec4f(0.f,0.f,0.f,1.f));
				dst.clear(Clear::All);

				m_gBufferPass->render(viewMtx, projMtx, m_opaqueQueue, m_rasterOptions, Material::Flags::Normals | Material::Flags::Shading, dst);
				m_gBufferMaskedPass->render(viewMtx, projMtx, m_alphaMaskQueue, m_rasterOptions, Material::Flags::Normals | Material::Flags::Shading | Material::Flags::AlphaMask, dst);
			});

		if (useEmissive)
		{
			frameGraph.addPass("G-Buffer + emissive",
				m_viewportSize,
				// Pass definition
				[&](RenderGraph::IPassBuilder& pass) {
					normals = pass.write(normals);
					albedo = pass.write(albedo);
					pbr = pass.write(pbr);
					depth = pass.write(depth);
					hdr = pass.write(hdr);
				},
				// Pass evaluation
					[&](const Texture2d* inputTextures, size_t nInputTextures, CommandBuffer& dst)
				{
					m_gBufferPass->render(viewMtx, projMtx, m_emissiveQueue, m_rasterOptions,
						Material::Flags::Normals | Material::Flags::Shading | Material::Flags::Emissive,
						dst);
					auto maskedOptions = m_rasterOptions;
					maskedOptions.alphaMask = true;
					m_gBufferMaskedPass->render(viewMtx, projMtx, m_emissiveMaskQueue, maskedOptions,
						Material::Flags::Normals | Material::Flags::Shading | Material::Flags::Emissive | Material::Flags::AlphaMask,
						dst);
				});
		}

		// AO pass
		RenderGraph::BufferResource ao; // G-Pass outputs
		frameGraph.addPass("AO-Sample",
			m_viewportSize/2u,
			// Pass definition
			[&](RenderGraph::IPassBuilder& pass) {
				ao = pass.write(BufferFormat::R8);
				pass.read(normals, 0);
				pass.read(depth, 1);
			},
			// Pass evaluation
				[&](const Texture2d* inputTextures, size_t nInputTextures, CommandBuffer& dst)
			{
				// Clear depth
				dst.clearColor(Vec4f::ones());
				dst.clear(Clear::Color);

				auto projection = eye.projection(aspectRatio);
				Mat44f invView = eye.world().matrix();

				CommandBuffer::UniformBucket uniforms;
				uniforms.addParam(0, projection);
				uniforms.addParam(1, invView);
				uniforms.addParam(2, math::Vec4f(
					float(m_viewportSize.x()), float(m_viewportSize.y()), // G-buffer size
					float(m_viewportSize.x()/2), float(m_viewportSize.y()/2) // AO buffer size
				));
				// Textures
				uniforms.addParam(7, inputTextures[0]);
				uniforms.addParam(8, inputTextures[1]);
				unsigned noiseTextureNdx = m_noisePermutations(m_rng); // New noise permutation for primary light
				uniforms.addParam(9, m_blueNoise[noiseTextureNdx]);

				m_aoSamplePass->render(uniforms, dst);
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
					auto prevMetrics = dst.metrics();
					dst.clearDepth(0.f);
					dst.clear(Clear::Depth);
					auto& light = *scene.lights()[0];
					m_shadowPass->render(m_renderQueue, m_visibleVolume, aspectRatio, eye, light, dst);
					
					// Debug metrics
					if (ImGui::CollapsingHeader("Shadow pass metrics:"))
					{
						(dst.metrics() - prevMetrics).draw();
					}
				});
		}

		// Environment light pass
		frameGraph.addPass("Light pass",
			m_viewportSize,
			// Pass definition
			[&](RenderGraph::IPassBuilder& pass) {
			//hdr = pass.write(BufferFormat::RGBA32);
				if (useEmissive)
					hdr = pass.write(hdr);
				else
					hdr = pass.write(BufferFormat::RGBA32); // Should write to hdr

				pass.read(normals, 0);
				pass.read(albedo, 1);
				pass.read(pbr, 2);
				pass.read(depth, 3);
				pass.read(ao, 4);
				// TODO: Shadows enabled? read them!
				if (useShadows)
					pass.read(shadows, 5);
			},
			// Pass evaluation
			[&](const Texture2d* inputTextures, size_t nInputTextures, CommandBuffer& dst)
			{
				if (!useEmissive)
				{
					dst.clearColor(Vec4f(0.f,0.f,0.f,1.f));
					dst.clear(Clear::Color);
				}

				// Light-pass
				if (auto env = scene.environment())
				{
					CommandBuffer::UniformBucket envUniforms;
					auto projection = eye.projection(aspectRatio);
					Mat44f invView = eye.world().matrix();
					envUniforms.addParam(0, projection);
					envUniforms.addParam(1, invView);
					envUniforms.addParam(2, math::Vec4f(float(m_viewportSize.x()), float(m_viewportSize.y()), 0.f, 0.f));

					envUniforms.addParam(4, env->texture());
					envUniforms.addParam(5, m_brdfIbl);
					envUniforms.addParam(6, (float)env->numLevels());

					envUniforms.addParam(7, inputTextures[0]);
					envUniforms.addParam(8, inputTextures[3]);
					envUniforms.addParam(9, inputTextures[2]);
					envUniforms.addParam(10, inputTextures[1]);
					envUniforms.addParam(13, inputTextures[4]);

					if (useShadows)
					{
						math::Mat44f shadowProj = m_shadowPass->shadowProj();
						envUniforms.addParam(11, inputTextures[5]);
						envUniforms.addParam(12, shadowProj);
					}

					m_lightingPass->render(envUniforms, dst);

					// Background
					// Uniforms
					CommandBuffer::UniformBucket bgUniforms;
					bgUniforms.mat4s.push_back({ 0, invView });
					bgUniforms.mat4s.push_back({ 1, projection });
					bgUniforms.vec4s.push_back({ 2, math::Vec4f(float(m_viewportSize.x()), float(m_viewportSize.y()), 0.f, 0.f) });
					bgUniforms.textures.push_back({ 7, env->texture() });
					// Render
					m_bgPass->render(bgUniforms, dst);
				}
			});

		// Transparent pass
		frameGraph.addPass("Transparent",
			m_viewportSize,
			[&](RenderGraph::IPassBuilder& pass) {
				hdr = pass.write(hdr);
				pass.read(depth, 0);
			},
			[&](const Texture2d* inputTextures, size_t nInputTextures, CommandBuffer& dst)
			{
				auto maskedOptions = m_rasterOptions;
				maskedOptions.blendMode = Pipeline::BlendMode::Additive;
				m_gTransparentPass->render(viewMtx, projMtx, m_transparentQueue, maskedOptions,
					Material::Flags::Normals | Material::Flags::Shading | Material::Flags::AlphaBlend,
					dst);
			});

		frameGraph.addPass("hdr",
			m_viewportSize,
			// Pass definition
			[&](RenderGraph::IPassBuilder& pass) {
				//hdr = pass.write(BufferFormat::RGBA32);
				pass.read(hdr, 0); // Should write to hdr
				pass.write(m_targetFb); // Hack: Doesn�t really write to it. But we need it bound in the fb
			},
			// Pass evaluation
				[&](const Texture2d* inputTextures, size_t nInputTextures, CommandBuffer& dst)
			{
				CommandBuffer::UniformBucket uniforms;
				uniforms.addParam(2, math::Vec4f(float(m_viewportSize.x()), float(m_viewportSize.y()), 0.f, 0.f));
				uniforms.addParam(3, powf(2.f, eye.exposure()));
				uniforms.addParam(4, inputTextures[0]);

				m_hdrPass->render(uniforms, dst);
			});


		// Record passes
		frameGraph.build(*m_fbCache);
		CommandBuffer frameCommands;

		frameGraph.evaluate(frameCommands);
		// Submit
		m_device->renderQueue().submitCommandBuffer(frameCommands);

		ImGui::Separator();
		ImGui::Text("Global performance counters");
		m_device->renderQueue().drawPerformanceCounters();

		ImGui::End();
	}

	//----------------------------------------------------------------------------------------------
	void DeferredRenderer::onResizeTarget(const math::Vec2u& _newSize)
	{
		// Skip it for now
		m_fbCache->deallocateResources();
		m_viewportSize = _newSize;
		m_shadowSize.x() = std::min(2048u, _newSize.y() * 2);
		m_shadowSize.y() = m_shadowSize.x();
	}

	//----------------------------------------------------------------------------------------------
	void DeferredRenderer::createRenderPasses(gfx::FrameBuffer target)
	{
		m_rasterOptions.cullBack = true;
		m_rasterOptions.depthTest = Pipeline::DepthTest::Gequal;

		ShaderCodeFragment* gBufferCode = ShaderCodeFragment::loadFromFile("shaders/gbuffer.fx");
		m_gBufferPass = std::make_unique<GeometryPass>(*m_device, gBufferCode);
		ShaderCodeFragment* gBufferMaskedCode = new ShaderCodeFragment(new ShaderCodeFragment("#define ALPHA_MASK\n"), gBufferCode);
		m_gBufferMaskedPass = std::make_unique<GeometryPass>(*m_device, gBufferMaskedCode);

		// Shadow pass
		m_shadowPass = std::make_unique<ShadowMapPass>(*m_device, m_shadowSize);

		// Lighting pass
		m_lightingPass = new FullScreenPass(*m_device, ShaderCodeFragment::loadFromFile("shaders/lightPass.fx"), Pipeline::DepthTest::Less, false, Pipeline::BlendMode::Additive);

		// Background pass
		m_bgPass = std::make_unique<FullScreenPass>(*m_device, ShaderCodeFragment::loadFromFile("shaders/sky.fx"), Pipeline::DepthTest::Gequal, false);

		m_aoSamplePass = std::make_unique<FullScreenPass>(*m_device, ShaderCodeFragment::loadFromFile("shaders/aoSample.fx"), Pipeline::DepthTest::Less, false);

		// Transparent
		ShaderCodeFragment* fwdCode = ShaderCodeFragment::loadFromFile("shaders/forward.fx");
		m_gTransparentPass = std::make_unique<GeometryPass>(*m_device, fwdCode);

		// HDR pass
		m_hdrPass = std::make_unique<FullScreenPass>(*m_device, ShaderCodeFragment::loadFromFile("shaders/hdr.fx"));
	}

	//------------------------------------------------------------------------------------------------------------------
	void DeferredRenderer::loadNoiseTextures()
	{
		// Noise texture sampler
		TextureSampler::Descriptor noiseSamplerDesc;
		noiseSamplerDesc.filter = TextureSampler::MinFilter::Nearest;
		noiseSamplerDesc.wrapS = TextureSampler::Wrap::Repeat;
		noiseSamplerDesc.wrapT = TextureSampler::Wrap::Repeat;

		rev::gfx::Texture2d::Descriptor m_noiseDesc;
		m_noiseDesc.sampler = m_device->createTextureSampler(noiseSamplerDesc);
		m_noiseDesc.depth = false;
		m_noiseDesc.mipLevels = 1;
		m_noiseDesc.pixelFormat.channel = Image::ChannelFormat::Byte;
		m_noiseDesc.pixelFormat.numChannels = 4;
		m_noiseDesc.size = Vec2u(64, 64);
		m_noiseDesc.sRGB = false;

		std::string imageName = "../data/blueNoise/LDR_RGBA_00.png";
		auto digitPos = imageName.find("00");
		for (unsigned i = 0; i < NumBlueNoiseTextures; ++i)
		{
#if 0
			// Create a brand new noise texture
			Vec4f * noise = new Vec4f[64 * 64];
			std::uniform_real_distribution<float> noiseDistrib;
			for (int i = 0; i < 64 * 64; ++i)
			{
				noise[i].x() = noiseDistrib(m_rng);
				noise[i].y() = noiseDistrib(m_rng);
				noise[i].z() = noiseDistrib(m_rng);
				noise[i].w() = noiseDistrib(m_rng);
			}
			m_noiseDesc.srcImages.emplace_back(new rev::gfx::Image(m_noiseDesc.size, noise));
			m_blueNoise[i] = m_device->createTexture2d(m_noiseDesc);
#else
			// Load image from file
			imageName[digitPos] = (i / 10) + '0';
			imageName[digitPos+1] = (i % 10) + '0';
			auto image = Image::load(imageName, 0);
			m_noiseDesc.srcImages.clear();
			if (image)
			{
				m_noiseDesc.srcImages.push_back(std::move(image));
				m_blueNoise[i] = m_device->createTexture2d(m_noiseDesc);
			}
#endif
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void DeferredRenderer::collapseSceneRenderables(const RenderScene& scene, const Camera& eye)
	{
		m_renderQueue.clear();
		m_visibleQueue.clear();
		// Keep visible volume AABB
		float minShadowDistance;
		m_visibleVolume.clear();

		// Cull renderables in view space for maximun compactness
		for(auto obj : scene.renderables())
		{			
			if(!obj->visible)
				continue;
			auto viewFromObj = m_cullingViewMtx * obj->transform;

			for(auto mesh : obj->mesh->mPrimitives)
			{
				assert(mesh.first && mesh.second);
				auto renderItem = RenderItem{ obj->transform, &*mesh.first, &*mesh.second };
				m_renderQueue.push_back(renderItem);

				AABB viewSpaceBB = viewFromObj * renderItem.geom->bbox();
				if (math::intersect(m_cullingFrustum, viewSpaceBB))
				{
					m_visibleQueue.push_back(renderItem);
					m_visibleVolume.add(viewSpaceBB);
				}
			}
		}

		// Clamp visible volume to the view frustum visibility range
		m_visibleVolume = m_visibleVolume.intersection(m_cullingFrustum.boundingBox());
	}

	//---------------------------------------------------------------------------------------------------------------------
	void DeferredRenderer::sortVisibleQueue()
	{
		Vec3f viewDir = -m_cullingViewMtx.transpose().col<2>().xyz();

		std::sort(m_visibleQueue.begin(), m_visibleQueue.end(), [=](const RenderItem& a, const RenderItem& b) {
			Vec3f aCenter = a.world.block<3, 3, 0, 0>() * a.geom->bbox().center();
			Vec3f bCenter = b.world.block<3, 3, 0, 0>() * b.geom->bbox().center();
			float da = dot(aCenter, viewDir);
			float db = dot(bCenter, viewDir);

			bool result = da < db;
			return result;
		});
	}

} // namespace rev::gfx*/