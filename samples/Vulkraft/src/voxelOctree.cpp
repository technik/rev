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
	uint8_t getVoxel(const std::vector<uint8_t>& layer, int sideSize, int x, int y, int z)
	{
		return layer[((x*sideSize)+y)*sideSize+z];
	}

	//------------------------------------------------------------------------------------------------------------------
	uint8_t& getVoxel(std::vector<uint8_t>& layer, int sideSize, int x, int y, int z)
	{
		return layer[((x*sideSize)+y)*sideSize+z];
	}

	//------------------------------------------------------------------------------------------------------------------
	VoxelOctree::VoxelOctree(rev::gfx::Device& gpu, const FullGrid& rawData)
		: m_gpu(gpu)
	{
		// Naive approach to SVO construction
		int treeDepth = rawData.depth;
		assert(treeDepth > 0);
		std::vector<std::vector<uint8_t>> layers(treeDepth);

		// Each layer contains only parent nodes
		for(int i = 0; i < treeDepth; ++i)
			layers[i].resize(1<<(3*i));
		
		// Populate layers
		const std::vector<uint8_t>* prevLayer = &rawData.voxels;
		for(int i = layers.size()-1; i >= 0; --i)
		{
			int sideSize = 1<<i;
			int axisMask = ~(int(-1)<<(i+1));

			for(int parent = 0; parent < layers[i].size(); ++parent)
			{
				int x = parent>>(2*sideSize);
				int y = (parent>>sideSize) & axisMask;
				int z = parent & axisMask;

				uint8_t validMask = 0;
				for(uint8_t child = 0; child < 8; ++child)
				{
					int cx = 2*x+(child>>2);
					int cy = 2*y+((child>>1)&1);
					int cz = 2*z+(child&1);
					if(getVoxel(*prevLayer, sideSize*2, cx, cy, cz))
						validMask |= 1<<child;
				}
				getVoxel(layers[i], sideSize, x,y,z) = validMask;
			}
			prevLayer = & layers[i];
		}

		// Build tree recursively out of the layers (breadth first)
		std::vector<int32_t> nodes;
		int childOffset = 1;
		for(int layerNdx = 0; layerNdx < layers.size(); ++ layerNdx)
		{
			const auto& layer = layers[layerNdx];
			int sideSize = 1<<layerNdx;
			int axisMask = ~(int(-1)<<(layerNdx+1));
			for(int i = 0; i < layer.size(); ++i)
			{
				assert(childOffset > 0);
				--childOffset;

				int node = layer[i];
				if(node != 0) // Node has children, so it needs to be stored, and needs an offset
				{
					if(layerNdx == layers.size()-1)
						node |= node<<8; // Leaf mask
					// Child offset
					assert(childOffset < uint16_t(-1));
					node |= childOffset<<16;
					// Increase child offset for next node by the number of children this node has.
					for(int i = 0; i < 8; ++i)
						childOffset += node&i ? 1:0;
					nodes.push_back(node);
				}

			}
		}

		// Send tree to the gpu
		m_gpuBuffer = m_gpu.allocateStorageBuffer(sizeof(int32_t)*nodes.size(), nodes.data());
	}

	//------------------------------------------------------------------------------------------------------------------
	void VoxelOctree::generateGrid(int depth, FullGrid& dst)
	{
		int sideSize = 1<<depth;
		assert(sideSize <= 1<<10);

		dst.depth = depth;
		dst.voxels.resize(sideSize*sideSize*sideSize);

		for(int i = 0; i < sideSize; ++i)
		{
			int planeOffset = i*sideSize*sideSize;
			for(int j = 0; j < sideSize; ++j)
			{
				int rowOffset = planeOffset + j*sideSize;
				for(int k = 0; k < sideSize; ++k)
				{
					int index = rowOffset + k;
					dst.voxels[index] = i <= j ? 1 : 0;
				}
			}
		}
	}
}