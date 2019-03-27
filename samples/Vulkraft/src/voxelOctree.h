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
		struct FullGrid
		{
			int depth;
			std::vector<uint8_t> voxels;
		};

		VoxelOctree(rev::gfx::Device& gpu, const FullGrid& rawData);

		rev::gfx::Buffer gpuBuffer() const { return m_gpuBuffer; }
		
		static void generateGrid(int depth, FullGrid& dst);

	private:
		rev::gfx::Device& m_gpu;
		rev::gfx::Buffer m_gpuBuffer;
	};
}
