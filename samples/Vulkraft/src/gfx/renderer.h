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

namespace vkft {
	class VoxelOctree;
}

namespace vkft::gfx
{

	class Renderer
	{
	public:
		Renderer(rev::gfx::DeviceOpenGLWindows&, const rev::math::Vec2u& targetSize);

		void render(const VoxelOctree&, const rev::gfx::Camera&, float dt);

		void onResizeTarget(const rev::math::Vec2u& targetSize);

	private:
		void loadNoiseTextures();
		void loadTexturePack();
        void loadShaderAndSetListener(const std::string& shaderFile, rev::gfx::ComputeShader& dst);

		// Filtering stages
		void Renderer::BoxFilter(
			rev::gfx::Texture2d texture,
			const rev::math::Vec4f& textureSize,
			unsigned iterations,
			rev::gfx::CommandBuffer& commands);

		void Renderer::TaaFilter(
			const rev::math::Vec4f& textureSize,
			float taaConfidence,
			rev::gfx::Texture2d gBuffer,
			rev::gfx::Texture2d pastFrame,
			rev::gfx::Texture2d thisFrame,
			rev::gfx::Texture2d target,
			rev::gfx::CommandBuffer& commands);

	private:
        // Noise
		static constexpr unsigned NumBlueNoiseTextures = 64;
		rev::gfx::Texture2d m_blueNoise[NumBlueNoiseTextures];
		std::default_random_engine m_rng;
		std::uniform_int_distribution<unsigned> m_noisePermutations;

        // Buffers
        rev::gfx::Texture2d m_gBufferTexture;
		rev::gfx::Texture2d m_directLightTexture;
		rev::gfx::Texture2d m_directLightTAABuffer[2];
		rev::gfx::Texture2d m_indirectLightTAABuffer[2];
        rev::gfx::Texture2d m_indirectLightTexture;
        rev::gfx::Texture2d m_raytracingTexture;
		rev::gfx::Texture2d m_pingPongTexture; // Used as a reusable target buffer for filtering
		int m_taaIndex = 0;
		float m_taaConfidence = 0.f;

        // Compute programs
        std::vector<rev::gfx::ShaderCodeFragment*> m_computeCode; // Code fragments for all compute programs
        rev::gfx::ComputeShader m_gBufferCompute;
        rev::gfx::ComputeShader m_directLightCompute;
        rev::gfx::ComputeShader m_composeCompute;
        rev::gfx::ComputeShader m_mixHorCompute;
		rev::gfx::ComputeShader m_mixVerCompute;
		rev::gfx::ComputeShader m_taaCompute;

		rev::gfx::TextureSampler m_rtBufferSampler;

		rev::gfx::ShaderCodeFragment* m_rasterCode = nullptr;
        using ShaderReloadListener = std::shared_ptr<rev::gfx::ShaderCodeFragment::ReloadListener>;
        std::vector<ShaderReloadListener> m_reloadListeners;
		rev::gfx::FullScreenPass m_rasterPass;
		rev::gfx::RenderPass* m_finalPass = nullptr;

		rev::gfx::DeviceOpenGLWindows& mGfxDevice;

		// Render parameters
		float mTargetFov;
		rev::math::Mat44f m_taaView;
		rev::math::Vec2u m_targetSize;
		rev::gfx::Texture2d m_texturePack;
	};
}