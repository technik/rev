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

	private:
		static constexpr unsigned NumBlueNoiseTextures = 64;
		rev::gfx::Texture2d m_blueNoise[NumBlueNoiseTextures];

		rev::gfx::ComputeShader m_raytracer;
		rev::gfx::Texture2d m_raytracingTexture;
		rev::gfx::Texture2d m_taaAccumTexture;
		bool m_freshTaa = true;
		rev::math::Mat44f m_oldCamWorld;
		rev::gfx::TextureSampler m_rtBufferSampler;

		// Noise
		std::default_random_engine m_rng;
		std::uniform_int_distribution<unsigned> m_noisePermutations;

		rev::gfx::ShaderCodeFragment* m_rasterCode = nullptr;
		rev::gfx::ShaderCodeFragment * m_computeCode = nullptr;
		std::shared_ptr<rev::gfx::ShaderCodeFragment::ReloadListener> m_computeReloadListener;
		std::shared_ptr<rev::gfx::ShaderCodeFragment::ReloadListener> m_rasterReloadListener;
		rev::gfx::FullScreenPass m_rasterPass;
		rev::gfx::RenderPass* m_finalPass = nullptr;

		rev::gfx::DeviceOpenGLWindows& mGfxDevice;

		// Render parameters
		float mTargetFov;
		rev::math::Vec2u m_targetSize;
	};
}