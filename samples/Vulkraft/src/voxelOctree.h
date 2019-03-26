//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
// Minecraft-style sample game
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <graphics/backend/device.h>

namespace vkft
{
	class VoxelOctree
	{
	public:
		VoxelOctree(rev::gfx::Device& gpu);

		rev::gfx::Buffer gpuBuffer() const { return m_gpuBuffer; }

	private:
		rev::gfx::Device& m_gpu;
		rev::gfx::Buffer m_gpuBuffer;
	};
}
