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
		m_viewportSize = size;
		const unsigned shadowBufferSize = 1024;
		m_shadowSize = Vec2u(shadowBufferSize, shadowBufferSize);
		createBuffers();
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
	std::string DeferredRenderer::vertexFormatDefines(RenderGeom::VtxFormat vertexFormat)
	{
		// TODO: Create this defines procedurally with more information from the actual rendergeom
		std::string defines;
		if(vertexFormat.position() == RenderGeom::VtxFormat::Storage::Float32)
			defines += "#define VTX_POSITION_FLOAT 0\n";
		if(vertexFormat.normal() == RenderGeom::VtxFormat::Storage::Float32)
			defines += "#define VTX_NORMAL_FLOAT 1\n";
		if(vertexFormat.tangent() == RenderGeom::VtxFormat::Storage::Float32)
			defines += "#define VTX_TANGENT_FLOAT 2\n";
		if(vertexFormat.uv() == RenderGeom::VtxFormat::Storage::Float32)
			defines += "#define VTX_UV_FLOAT 3\n";
		return defines;
	}

	ShaderCodeFragment* DeferredRenderer::getMaterialCode(RenderGeom::VtxFormat vtxFormat, const Material& material)
	{
		auto completeCode = vertexFormatDefines(vtxFormat) + material.bakedOptions() + material.effect().code();
		auto iter = m_materialCode.find(completeCode);
		if(iter == m_materialCode.end())
		{
			iter = m_materialCode.emplace(completeCode, new ShaderCodeFragment(completeCode.c_str())).first;
			material.effect().onReload([this](const Effect&){
				m_materialCode.clear();
			});
		}
		return iter->second;
	}

	//----------------------------------------------------------------------------------------------
	void DeferredRenderer::render(const RenderScene& scene, const Camera& eye)
	{
		collapseSceneRenderables(scene);// Consolidate renderables into geometry (i.e. extracts geom from renderObj)

		// --- Cull stuff
		// Cull visible objects renderQ -> visible
		//cull(m_renderQueue, m_visible, [&](const RenderItem& item) -> bool {
		//	return dot(item.world.position() - eye.position(), eye.viewDir()) > 0;
		//});

		auto worldMatrix = Mat44f::identity();
		GeometryPass::Instance instance;
		instance.geometryIndex = uint32_t(-1);
		instance.instanceCode = nullptr;

		std::vector<GeometryPass::Instance> renderList;
		std::vector<const RenderGeom*> geometry;
		const RenderGeom* lastGeom = nullptr;
		const Material* lastMaterial = nullptr;

		float aspectRatio = float(m_viewportSize.x())/m_viewportSize.y();
		auto viewProj = eye.viewProj(aspectRatio);

		for(auto& mesh : m_visible)
		{
			// Raster options
			bool mirroredGeometry = affineTransformDeterminant(worldMatrix) < 0.f;
			m_rasterOptions.frontFace = mirroredGeometry ? Pipeline::Winding::CW : Pipeline::Winding::CCW;
			instance.raster = m_rasterOptions.mask();
			// Uniforms
			instance.uniforms.clear();
			Mat44f world = mesh.world;
			Mat44f wvp = viewProj * world;
			instance.uniforms.mat4s.push_back({0, wvp});
			instance.uniforms.mat4s.push_back({1, world});
			// Geometry
			if((lastGeom != &mesh.geom) || (lastMaterial != &mesh.material))
			{
				lastMaterial = &mesh.material;
				lastGeom = &mesh.geom;
				mesh.material.bindParams(instance.uniforms, Material::BindingFlags(Material::BindingFlags::Normals|Material::BindingFlags::PBR));
				instance.instanceCode = getMaterialCode(mesh.geom.vertexFormat(), mesh.material);
				instance.geometryIndex++;
				geometry.push_back(&mesh.geom);
			}
			renderList.push_back(instance);
		}

		// Record passes
		CommandBuffer frameCommands;

		// Shadows
		const bool useShadows = !scene.lights().empty() && scene.lights()[0]->castShadows;
		if (useShadows)
		{
			auto& light = *scene.lights()[0];
			m_shadowPass->render(m_visible, m_visible, eye, light, frameCommands);
		}

		// G-pass
		frameCommands.beginPass(*m_gBufferPass);
		m_gPass->render(geometry, renderList, frameCommands);
		
		// Light-pass
		frameCommands.beginPass(*m_lPass);
		if(auto env = scene.environment())
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

			envUniforms.addParam(7, m_gBufferTexture);
			envUniforms.addParam(8, m_depthTexture);
			envUniforms.addParam(9, m_specularTexture);
			envUniforms.addParam(10, m_albedoTexture);

			if (useShadows)
			{
				math::Mat44f shadowProj = m_shadowPass->shadowProj();
				envUniforms.addParam(11, m_shadowTexture);
				envUniforms.addParam(12, shadowProj);
			}
			
			m_lightingPass->render(envUniforms, frameCommands);

			// Background
			// Uniforms
			CommandBuffer::UniformBucket bgUniforms;
			bgUniforms.mat4s.push_back({0, invView });
			bgUniforms.mat4s.push_back({1, projection });
			bgUniforms.vec4s.push_back({2, math::Vec4f(float(m_viewportSize.x()), float(m_viewportSize.y()), 0.f, 0.f) });
			bgUniforms.floats.push_back({3, 0.f }); // Neutral exposure
			bgUniforms.textures.push_back({7, env->texture()} );
			// Render
			m_bgPass->render(bgUniforms, frameCommands);
		}

		// Submit
		m_device->renderQueue().submitCommandBuffer(frameCommands);
	}

	//----------------------------------------------------------------------------------------------
	void DeferredRenderer::onResizeTarget(const math::Vec2u& _newSize)
	{
		// Skip it for now
		m_viewportSize = _newSize;
		m_gBufferPass->setViewport({0,0}, _newSize);
		m_lPass->setViewport({0,0}, _newSize);

		// Recreate internal buffers
		m_device->destroyTexture2d(m_gBufferTexture);
		m_device->destroyTexture2d(m_depthTexture);
		m_device->destroyTexture2d(m_albedoTexture);
		m_device->destroyTexture2d(m_specularTexture);
		createBuffers();
		if(m_gPass)
			delete m_gPass;
		if(m_gBufferPass)
			delete m_gBufferPass;
		if(m_lightingPass)
			delete m_lightingPass;
		if(m_lPass)
			delete m_lPass;
		createRenderPasses(m_targetFb);
	}

	//----------------------------------------------------------------------------------------------
	void DeferredRenderer::createBuffers()
	{
		m_gBufferTexture = createGBufferTexture(*m_device, m_viewportSize);
		m_depthTexture = createDepthTexture(*m_device, m_viewportSize);
		createPBRTextures(*m_device, m_viewportSize);
		mGBuffer = createGBuffer(*m_device, m_depthTexture, m_gBufferTexture);

		m_shadowTexture = ShadowMapPass::createShadowMapTexture(*m_device, m_shadowSize);
		m_shadowBuffer = ShadowMapPass::createShadowBuffer(*m_device, m_shadowTexture);
	}

	//----------------------------------------------------------------------------------------------
	void DeferredRenderer::createRenderPasses(gfx::FrameBuffer target)
	{
		m_rasterOptions.cullBack = true;
		m_rasterOptions.depthTest = Pipeline::DepthTest::Gequal;

		// G-Buffer pass
		RenderPass::Descriptor passDesc;
		passDesc.clearDepth = 0;
		passDesc.clearFlags = RenderPass::Descriptor::Clear::All;
		passDesc.target = mGBuffer;
		passDesc.numColorAttachs = 3;
		passDesc.viewportSize = m_viewportSize;

		m_gBufferPass = m_device->createRenderPass(passDesc);
		m_gPass = new GeometryPass(*m_device, ShaderCodeFragment::loadFromFile("shaders/gbuffer.fx"));

		// Shadow pass
		m_shadowPass = std::make_unique<ShadowMapPass>(*m_device, m_shadowBuffer, m_shadowSize);

		// Lighting pass
		passDesc.clearDepth = 0;
		passDesc.clearFlags = RenderPass::Descriptor::Clear::Depth;
		passDesc.target = target;
		passDesc.numColorAttachs = 1;
		passDesc.sRGB = true;
		passDesc.viewportSize = m_viewportSize;
		m_lPass = m_device->createRenderPass(passDesc);
		m_lightingPass = new FullScreenPass(*m_device, ShaderCodeFragment::loadFromFile("shaders/lightPass.fx"));

		// Background pass
		passDesc.clearFlags = RenderPass::Descriptor::Clear::None;
		passDesc.target = target;
		passDesc.viewportSize = m_viewportSize;
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

	//----------------------------------------------------------------------------------------------
	Texture2d DeferredRenderer::createGBufferTexture(Device& device, const math::Vec2u& size)
	{
		auto samplerDesc = TextureSampler::Descriptor();
		samplerDesc.wrapS = TextureSampler::Wrap::Clamp;
		samplerDesc.wrapT = TextureSampler::Wrap::Clamp;
		samplerDesc.filter = TextureSampler::MinFilter::Linear;
		auto sampler = device.createTextureSampler(samplerDesc);
		auto textureDesc = Texture2d::Descriptor();
		textureDesc.pixelFormat.channel = Image::ChannelFormat::Float32;
		textureDesc.pixelFormat.numChannels = 4;
		textureDesc.depth = false;
		textureDesc.sampler = sampler;
		textureDesc.mipLevels = 1;
		textureDesc.size = size;

		return device.createTexture2d(textureDesc);
	}

	//----------------------------------------------------------------------------------------------
	void DeferredRenderer::createPBRTextures(Device& device, const math::Vec2u& size)
	{
		auto samplerDesc = TextureSampler::Descriptor();
		samplerDesc.wrapS = TextureSampler::Wrap::Clamp;
		samplerDesc.wrapT = TextureSampler::Wrap::Clamp;
		samplerDesc.filter = TextureSampler::MinFilter::Linear;
		auto sampler = device.createTextureSampler(samplerDesc);
		auto textureDesc = Texture2d::Descriptor();
		textureDesc.pixelFormat.channel = Image::ChannelFormat::Byte;
		textureDesc.pixelFormat.numChannels = 4;
		textureDesc.depth = false;
		textureDesc.sampler = sampler;
		textureDesc.mipLevels = 1;
		textureDesc.size = size;

		m_albedoTexture = device.createTexture2d(textureDesc);
		m_specularTexture = device.createTexture2d(textureDesc);
	}

	//----------------------------------------------------------------------------------------------
	Texture2d DeferredRenderer::createDepthTexture(Device& device, const math::Vec2u& size)
	{
		auto samplerDesc = TextureSampler::Descriptor();
		samplerDesc.wrapS = TextureSampler::Wrap::Clamp;
		samplerDesc.wrapT = TextureSampler::Wrap::Clamp;
		samplerDesc.filter = TextureSampler::MinFilter::Linear;
		auto sampler = device.createTextureSampler(samplerDesc);
		auto textureDesc = Texture2d::Descriptor();
		textureDesc.pixelFormat.channel = Image::ChannelFormat::Float32;
		textureDesc.pixelFormat.numChannels = 1;
		textureDesc.depth = true;
		textureDesc.sampler = sampler;
		textureDesc.mipLevels = 1;
		textureDesc.size = size;

		return device.createTexture2d(textureDesc);
	}

	//----------------------------------------------------------------------------------------------
	FrameBuffer DeferredRenderer::createGBuffer(Device& device, Texture2d depth, Texture2d normal)
	{
		gfx::FrameBuffer::Attachment attachments[4];
		attachments[0].target = gfx::FrameBuffer::Attachment::Target::Depth;
		attachments[0].texture = depth;
		attachments[1].target = gfx::FrameBuffer::Attachment::Target::Color;
		attachments[1].texture = normal;
		attachments[2].target = gfx::FrameBuffer::Attachment::Target::Color;
		attachments[2].texture = m_albedoTexture;
		attachments[3].target = gfx::FrameBuffer::Attachment::Target::Color;
		attachments[3].texture = m_specularTexture;
		gfx::FrameBuffer::Descriptor gBufferDesc;
		gBufferDesc.numAttachments = 4;
		gBufferDesc.attachments = attachments;
		return device.createFrameBuffer(gBufferDesc);
	}

} // namespace rev::gfx