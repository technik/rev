//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
// Minecraft-style sample game
//----------------------------------------------------------------------------------------------------------------------
#include "voxelOctree.h"

namespace vkft
{
	// Static representation
	int staticTree[5+4*8] =
	{
		0<<16 | 0x00<<8 | 0x33, // Top level
		3<<16 | 0x00<<8 | 0xff, // Mid level
		10<<16 | 0x00<<8 | 0xff, // Mid level
		17<<16 | 0x00<<8 | 0xff, // Mid level
		24<<16 | 0x00<<8 | 0xff, // Mid level
		// Cube a
		0xff<<8 | 0x57,
		0xff<<8 | 0x0b,
		0xff<<8 | 0x93,
		0xff<<8 | 0xc3,
		0xff<<8 | 0x55,
		0xff<<8 | 0x00,
		0xff<<8 | 0x99,
		0xff<<8 | 0x4c,
		// Cube b
		0xff<<8 | 0x07,
		0xff<<8 | 0xab,
		0xff<<8 | 0xc3,
		0xff<<8 | 0x63,
		0xff<<8 | 0x00,
		0xff<<8 | 0x00,
		0xff<<8 | 0x8c,
		0xff<<8 | 0x66,
		// Cube c
		0xff<<8 | 0x55,
		0xff<<8 | 0x00,
		0xff<<8 | 0x99,
		0xff<<8 | 0xc4,
		0xff<<8 | 0x75,
		0xff<<8 | 0xb0,
		0xff<<8 | 0x39,
		0xff<<8 | 0x3c,
		// Cube d
		0xff<<8 | 0x00,
		0xff<<8 | 0x2a,
		0xff<<8 | 0xc8,
		0xff<<8 | 0x66,
		0xff<<8 | 0x70,
		0xff<<8 | 0xb2,
		0xff<<8 | 0x3c,
		0xff<<8 | 0x36,
	};

	//------------------------------------------------------------------------------------------------------------------
	VoxelOctree::VoxelOctree(rev::gfx::Device& gpu)
		: m_gpu(gpu)
	{
		m_gpuBuffer = m_gpu.allocateStorageBuffer(sizeof(staticTree), staticTree);
	}
}