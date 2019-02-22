//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
// Minecraft-style sample game
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <graphics/backend/device.h>
#include <graphics/backend/commandBuffer.h>
#include <graphics/renderer/renderPass/fullScreenPass.h>
#include <random>

namespace rev::gfx {
	class Camera;
	class DeviceOpenGLWindows; 
}

namespace vkft::gfx
{
	class World;

	class Renderer
	{
	public:
		Renderer(rev::gfx::DeviceOpenGLWindows&, const rev::math::Vec2u& targetSize);

		void render(const World&, const rev::gfx::Camera&);

		void onResizeTarget(const rev::math::Vec2u& targetSize);

	private:
		void loadNoiseTextures();
        void loadShaderAndSetListener(const std::string& shaderFile, rev::gfx::ComputeShader& dst);

	private:
        // Noise
		static constexpr unsigned NumBlueNoiseTextures = 64;
		rev::gfx::Texture2d m_blueNoise[NumBlueNoiseTextures];
		std::default_random_engine m_rng;
		std::uniform_int_distribution<unsigned> m_noisePermutations;

        // Buffers
        rev::gfx::Texture2d m_gBufferTexture;
        rev::gfx::Texture2d m_directLightTexture;
        rev::gfx::Texture2d m_indirectLightTexture;
        rev::gfx::Texture2d m_raytracingTexture;

        // Compute programs
        std::vector<rev::gfx::ShaderCodeFragment*> m_computeCode; // Code fragments for all compute programs
        rev::gfx::ComputeShader m_gBufferCompute;
        rev::gfx::ComputeShader m_directLightCompute;
        rev::gfx::ComputeShader m_indirectLightCompute;
        rev::gfx::ComputeShader m_composeCompute;

		rev::gfx::TextureSampler m_rtBufferSampler;

		rev::gfx::ShaderCodeFragment* m_rasterCode = nullptr;
        using ShaderReloadListener = std::shared_ptr<rev::gfx::ShaderCodeFragment::ReloadListener>;
        std::vector<ShaderReloadListener> m_reloadListeners;
		rev::gfx::FullScreenPass m_rasterPass;
		rev::gfx::RenderPass* m_finalPass = nullptr;

		rev::gfx::DeviceOpenGLWindows& mGfxDevice;

		// Render parameters
		float mTargetFov;
		rev::math::Vec2u m_targetSize;
	};
}