//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
// Minecraft-style sample game
//----------------------------------------------------------------------------------------------------------------------
#include "renderer.h"
#include "world.h"

#include <graphics/backend/OpenGL/deviceOpenGLWindows.h>
#include <graphics/backend/pipeline.h>
#include <graphics/backend/renderPass.h>
#include <graphics/backend/texture2d.h>
#include <graphics/scene/camera.h>
#include <graphics/Image.h>

#include <string>
#include <sstream>

using namespace rev::gfx;
using namespace rev::math;

namespace vkft::gfx
{
	//------------------------------------------------------------------------------------------------------------------
	Renderer::Renderer(rev::gfx::DeviceOpenGLWindows& device, const Vec2u& targetSize)
		: mGfxDevice(device)
		, m_noisePermutations(0,NumBlueNoiseTextures)
		, m_rasterPass(device)
	{
		loadNoiseTextures();
		// Create reusable texture descriptor
		TextureSampler::Descriptor bufferSamplerDesc;
		bufferSamplerDesc.filter = TextureSampler::MinFilter::Nearest;
		bufferSamplerDesc.wrapS = TextureSampler::Wrap::Clamp;
		bufferSamplerDesc.wrapT = TextureSampler::Wrap::Clamp;
		m_rtBufferSampler = mGfxDevice.createTextureSampler(bufferSamplerDesc);

		// Create an intermediate buffer of the right size
		onResizeTarget(targetSize);

		// Create the full screen pass to draw the result
		m_rasterCode = ShaderCodeFragment::loadFromFile("raster.fx");
		m_rasterReloadListener = m_rasterCode->onReload([this](ShaderCodeFragment& fragment){
				m_rasterPass.setPassCode(&fragment);
		});
		m_rasterPass.setPassCode(m_rasterCode);

		RenderPass::Descriptor fullScreenDesc;
		float grey = 0.5f;
		fullScreenDesc.clearColor = { grey,grey,grey, 1.f };
		fullScreenDesc.clearDepth = 1.0;
		fullScreenDesc.clearFlags = RenderPass::Descriptor::Clear::All;
		fullScreenDesc.target = mGfxDevice.defaultFrameBuffer();
		fullScreenDesc.viewportSize = targetSize;
		m_finalPass = mGfxDevice.createRenderPass(fullScreenDesc);

		// Show compute device limits on console
		auto& limits = mGfxDevice.getDeviceLimits();
		std::cout << "Compute group max count: " << limits.computeWorkGroupCount.x() << " " << limits.computeWorkGroupCount.y() << " " << limits.computeWorkGroupCount.z() << "\n";
		std::cout << "Compute group max size: " << limits.computeWorkGroupSize.x() << " " << limits.computeWorkGroupSize.y() << " " << limits.computeWorkGroupSize.z() << "\n";
		std::cout << "Compute max invokations: " << limits.computeWorkGruopTotalInvokes << "\n";

		// Create compute shader
		m_computeCode = ShaderCodeFragment::loadFromFile("Raytrace.fx");
		std::vector<std::string> code;
		m_computeCode->collapse(code);
		m_computeReloadListener = m_computeCode->onReload([this](ShaderCodeFragment& fragment){
			std::vector<std::string> code;
			fragment.collapse(code);
			auto newCompute = mGfxDevice.createComputeShader(code);
			if(newCompute.isValid())
				m_raytracer = newCompute;
		});

		m_raytracer = mGfxDevice.createComputeShader(code);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Renderer::render(const World& world, const rev::gfx::Camera& camera)
	{
		CommandBuffer commands;
		CommandBuffer::UniformBucket uniforms;

		if(!m_raytracer.isValid())
			return;
		
		// Prepare data
		Vec4f uWindow;
		uWindow.x() = (float)m_targetSize.x();
		uWindow.y() = (float)m_targetSize.y();

		// Dispatch gBuffer shader
		/*uniforms.clear();
		uniforms.addParam(1, uWindow);
		Mat44f curCamWorld = camera.world().matrix();
		uniforms.addParam(2, curCamWorld);
		unsigned noiseTextureNdx = m_noisePermutations(m_rng);
		uniforms.addParam(3, m_blueNoise[noiseTextureNdx]);
		commands.setComputeProgram(m_raytracer);
		commands.setUniformData(uniforms);
		commands.dispatchCompute(m_gBufferTexture, Vec3i{ (int)m_targetSize.x(), (int)m_targetSize.y(), 1});

		// Dispatch first shadow (direct light contrib)
		uniforms.clear();
		uniforms.addParam(1, uWindow);
		Mat44f curCamWorld = camera.world().matrix();
		uniforms.addParam(2, curCamWorld);
		unsigned noiseTextureNdx = m_noisePermutations(m_rng);
		uniforms.addParam(3, m_blueNoise[noiseTextureNdx]);
		commands.setComputeProgram(m_raytracer);
		commands.setUniformData(uniforms);
		commands.dispatchCompute(m_directLightTexture, Vec3i{ (int)m_targetSize.x(), (int)m_targetSize.y(), 1});

		// Dispatch indirect light
		uniforms.clear();
		uniforms.addParam(1, uWindow);
		Mat44f curCamWorld = camera.world().matrix();
		uniforms.addParam(2, curCamWorld);
		unsigned noiseTextureNdx = m_noisePermutations(m_rng);
		uniforms.addParam(3, m_blueNoise[noiseTextureNdx]);
		commands.setComputeProgram(m_raytracer);
		commands.setUniformData(uniforms);
		commands.memoryBarrier(CommandBuffer::MemoryBarrier::ImageAccess);
		commands.dispatchCompute(m_indirectLightTexture, Vec3i{ (int)m_targetSize.x(), (int)m_targetSize.y(), 1});*/

		// All following 3 at once?
		// Denoise shadow
		// Denoise indirect light
		// Compose image
		uniforms.clear();
		uniforms.addParam(1, uWindow);
		Mat44f curCamWorld = camera.world().matrix();
		uniforms.addParam(2, curCamWorld);
		unsigned noiseTextureNdx = m_noisePermutations(m_rng);
		uniforms.addParam(3, m_blueNoise[noiseTextureNdx]);
		/*uniforms.addParam(3, m_blueNoise[noiseTextureNdx]);
		uniforms.addParam(4, m_gBufferTexture);
		uniforms.addParam(5, m_directLightTexture);
		uniforms.addParam(6, m_indirectLightTexture);*/
		commands.setComputeProgram(m_raytracer);
		commands.setUniformData(uniforms);
		commands.memoryBarrier(CommandBuffer::MemoryBarrier::ImageAccess);
		commands.dispatchCompute(m_raytracingTexture, Vec3i{ (int)m_targetSize.x(), (int)m_targetSize.y(), 1});

		// Render full screen texture
		uniforms.clear();
		uniforms.addParam(0, uWindow);
		uniforms.addParam(1, m_raytracingTexture);
		commands.beginPass(*m_finalPass);
		commands.memoryBarrier(CommandBuffer::MemoryBarrier::ImageAccess);
		m_rasterPass.render(uniforms, commands);

		// Submit
		mGfxDevice.renderQueue().submitCommandBuffer(commands);
		mGfxDevice.renderQueue().present();

		// TAA for next frame
		m_oldCamWorld = curCamWorld;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Renderer::onResizeTarget(const rev::math::Vec2u& newSize)
	{
		m_targetSize = newSize;
		mTargetFov = float(newSize.x()) / newSize.y();
		if(m_finalPass)
			m_finalPass->setViewport({0,0}, newSize);

		// Delete previous target if needed
		if(m_raytracingTexture.isValid())
		{
			mGfxDevice.destroyTexture2d(m_raytracingTexture);
			mGfxDevice.destroyTexture2d(m_taaAccumTexture);
		}

		// Create the raytracing texture
		Texture2d::Descriptor bufferDesc;
		bufferDesc.depth = false;
		bufferDesc.mipLevels = 1;
		bufferDesc.pixelFormat.channel = Image::ChannelFormat::Float32;
		bufferDesc.pixelFormat.numChannels = 4;
		bufferDesc.providedImages = 0;
		bufferDesc.sampler = m_rtBufferSampler;
		bufferDesc.size = newSize;
		bufferDesc.sRGB = false;
		m_raytracingTexture = mGfxDevice.createTexture2d(bufferDesc);
		m_taaAccumTexture = mGfxDevice.createTexture2d(bufferDesc);
		m_freshTaa = true;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Renderer::loadNoiseTextures()
	{
		// Noise texture sampler
		TextureSampler::Descriptor noiseSamplerDesc;
		noiseSamplerDesc.filter = TextureSampler::MinFilter::Linear;
		noiseSamplerDesc.wrapS = TextureSampler::Wrap::Repeat;
		noiseSamplerDesc.wrapT = TextureSampler::Wrap::Repeat;

		rev::gfx::Texture2d::Descriptor m_noiseDesc;
		m_noiseDesc.sampler = mGfxDevice.createTextureSampler(noiseSamplerDesc);
		m_noiseDesc.depth = false;
		m_noiseDesc.mipLevels = 1;
		m_noiseDesc.pixelFormat.channel = Image::ChannelFormat::Float32;
		m_noiseDesc.pixelFormat.numChannels = 4;
		m_noiseDesc.providedImages = 1;
		m_noiseDesc.size = Vec2u(64,64);
		m_noiseDesc.sRGB = false;

		for(unsigned i = 0; i < NumBlueNoiseTextures; ++i)
		{
			// Load image from file
			std::stringstream imageName;
			imageName << "../assets/blueNoise/LDR_RGBA_" << i << ".png";
			auto image = Image::load(imageName.str(), 0);
			if(image.data())
			{
				m_noiseDesc.srcImages = &image;
				m_blueNoise[i] = mGfxDevice.createTexture2d(m_noiseDesc);
			}
		}
	}
}