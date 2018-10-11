//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
// Minecraft-style sample game
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <graphics/backend/device.h>
#include <graphics/backend/commandBuffer.h>

namespace rev::gfx { class DeviceOpenGLWindows; }

namespace vkft::gfx
{
	class World;

	class Renderer
	{
	public:
		Renderer(rev::gfx::DeviceOpenGLWindows&, const rev::math::Vec2u& targetSize);

		void render(const World&);

		void onResizeTarget(const rev::math::Vec2u& targetSize);

	private:

		void initForwardPass(const rev::math::Vec2u& targetSize);
		void initGeometryPipeline();

		rev::gfx::DeviceOpenGLWindows& mGfxDevice;

		rev::gfx::CommandBuffer::UniformBucket mUniforms;

		rev::gfx::Pipeline mFwdPipeline;
		rev::gfx::CommandBuffer mRenderCommands;
		rev::gfx::RenderPass* mForwardPass = nullptr;
	};
}