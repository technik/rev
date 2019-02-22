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
		m_reloadListeners.push_back(m_rasterCode->onReload([this](ShaderCodeFragment& fragment){
				m_rasterPass.setPassCode(&fragment);
		}));
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
        loadShaderAndSetListener("rtGBuffer.fx", m_gBufferCompute);
        loadShaderAndSetListener("rtDirect.fx", m_directLightCompute);
        loadShaderAndSetListener("rtIndirect.fx", m_indirectLightCompute);
        loadShaderAndSetListener("rtCompose.fx", m_composeCompute);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Renderer::render(const World& world, const rev::gfx::Camera& camera)
	{
		CommandBuffer commands;
        CommandBuffer::UniformBucket passUniforms;

		if(!m_composeCompute.isValid())
			return;

        // Optimization ideas:
        // - Run a broad phase conservative g-buffer to reduce distance traversed by primary rays
        // - Or just rasterize the g-buffer. Which also allows you to take advantage of normal mapping and all things
        //   that can be done on deferred.
        // - Indirect lighting would use raytraced geometry and discard things like normal mapping. Also simplified
        //   shading, and probably increased roughness. Could even use environment probes for terciary lighting
        // - For supporting many lights, do as clustered forward and precompute a list of affecting lights per cluster
        // - Can that list work for both direct and indirect lighting?
		
		// Prepare data
		Vec4f uWindow;
		uWindow.x() = (float)m_targetSize.x();
		uWindow.y() = (float)m_targetSize.y();
        CommandBuffer::UniformBucket commonUniforms;
        commonUniforms.addParam(1, uWindow);
		Mat44f curCamWorld = camera.world().matrix();
        commonUniforms.addParam(2, curCamWorld);

		// Dispatch gBuffer shader
		commands.setComputeProgram(m_gBufferCompute);
        commands.setUniformData(commonUniforms);
		commands.dispatchCompute(m_gBufferTexture, Vec3i{ (int)m_targetSize.x(), (int)m_targetSize.y(), 1});

		// Dispatch first shadow (direct light contrib)
        passUniforms.clear();
        passUniforms.addParam(3, m_gBufferTexture); // So we can read from the g-buffer
        unsigned noiseTextureNdx = m_noisePermutations(m_rng); // New noise permutation for primary light
        passUniforms.addParam(4, m_blueNoise[noiseTextureNdx]);
        commands.setComputeProgram(m_directLightCompute);
        commands.setUniformData(commonUniforms);
        commands.setUniformData(passUniforms);
        commands.memoryBarrier(CommandBuffer::MemoryBarrier::ImageAccess); // Wait for G-Buffer to be ready
        commands.dispatchCompute(m_directLightTexture, Vec3i{ (int)m_targetSize.x(), (int)m_targetSize.y(), 1 });

        // Dispatch indirect light
        passUniforms.clear();
        passUniforms.addParam(3, m_gBufferTexture); // So we can read from the g-buffer
        noiseTextureNdx = m_noisePermutations(m_rng); // New noise permutation for secondary light
        passUniforms.addParam(4, m_blueNoise[noiseTextureNdx]);
        commands.setComputeProgram(m_indirectLightCompute);
        commands.setUniformData(commonUniforms);
        commands.setUniformData(passUniforms);
        commands.dispatchCompute(m_indirectLightTexture, Vec3i{ (int)m_targetSize.x(), (int)m_targetSize.y(), 1 });

		// All following 3 at once:
		// Denoise direct light
		// Denoise indirect light
		// Compose image
		passUniforms.clear();
		noiseTextureNdx = m_noisePermutations(m_rng);
        passUniforms.addParam(3, m_blueNoise[noiseTextureNdx]);
        passUniforms.addParam(4, m_gBufferTexture);
        passUniforms.addParam(5, m_directLightTexture);
        passUniforms.addParam(6, m_indirectLightTexture);
		commands.setComputeProgram(m_composeCompute);
        commands.setUniformData(commonUniforms);
        commands.setUniformData(passUniforms);
		commands.memoryBarrier(CommandBuffer::MemoryBarrier::ImageAccess);
		commands.dispatchCompute(m_raytracingTexture, Vec3i{ (int)m_targetSize.x(), (int)m_targetSize.y(), 1});

		// Render full screen texture
        passUniforms.clear();
        passUniforms.addParam(0, uWindow);
        passUniforms.addParam(1, m_raytracingTexture);
		commands.beginPass(*m_finalPass);
		commands.memoryBarrier(CommandBuffer::MemoryBarrier::ImageAccess);
		m_rasterPass.render(passUniforms, commands);

		// Submit
		mGfxDevice.renderQueue().submitCommandBuffer(commands);
		mGfxDevice.renderQueue().present();
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
            mGfxDevice.destroyTexture2d(m_gBufferTexture);
            mGfxDevice.destroyTexture2d(m_directLightTexture);
            mGfxDevice.destroyTexture2d(m_indirectLightTexture);
            mGfxDevice.destroyTexture2d(m_raytracingTexture);
		}

		// Create a common descriptor for all buffers
		Texture2d::Descriptor bufferDesc;
		bufferDesc.depth = false;
		bufferDesc.mipLevels = 1;
		bufferDesc.pixelFormat.channel = Image::ChannelFormat::Float32;
		bufferDesc.pixelFormat.numChannels = 4;
		bufferDesc.providedImages = 0;
		bufferDesc.sampler = m_rtBufferSampler;
		bufferDesc.size = newSize;
		bufferDesc.sRGB = false;

        // Create the buffers
        m_gBufferTexture = mGfxDevice.createTexture2d(bufferDesc);
        m_directLightTexture = mGfxDevice.createTexture2d(bufferDesc);
        m_indirectLightTexture = mGfxDevice.createTexture2d(bufferDesc);
        m_raytracingTexture = mGfxDevice.createTexture2d(bufferDesc);
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

    //------------------------------------------------------------------------------------------------------------------
    void Renderer::loadShaderAndSetListener(const std::string& shaderFile, ComputeShader& dst)
    {
        m_computeCode.push_back(nullptr);
        auto& dstFragment = m_computeCode.back();
        dstFragment = ShaderCodeFragment::loadFromFile(shaderFile);
        std::vector<std::string> code;
        dstFragment->collapse(code);
        m_reloadListeners.emplace_back(dstFragment->onReload([this,&dst](ShaderCodeFragment& fragment) {
            std::vector<std::string> code;
            fragment.collapse(code);
            auto newCompute = mGfxDevice.createComputeShader(code);
            if (newCompute.isValid())
                dst = newCompute;
        }));

        dst = mGfxDevice.createComputeShader(code);
    }
}