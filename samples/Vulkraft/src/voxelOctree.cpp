//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
// Minecraft-style sample game
//----------------------------------------------------------------------------------------------------------------------
#include "voxelOctree.h"
#include <list>

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
			int axisMask = ~(int(-1)<<(i)); // i ones. eg, i=3 ->  0b0000'0111

			for(int parent = 0; parent < layers[i].size(); ++parent)
			{
				int x = parent>>(2*i);
				int y = (parent>>i) & axisMask;
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
		std::list<std::pair<uint8_t,int32_t>> childrenStack; // layer, index
		childrenStack.push_back({0,0});
		while(!childrenStack.empty())
		{
			const auto parent = childrenStack.front();
			childrenStack.pop_front();

			uint8_t layerNdx = parent.first;
			int32_t parentPos = parent.second;
			int32_t axisMask = ~(int32_t(-1)<<(layerNdx)); // i ones. eg, i=3 ->  0b0000'0111

			int x = parentPos>>(2*layerNdx);
			int y = (parentPos>>layerNdx) & axisMask;
			int z = parentPos & axisMask;

			assert(childrenStack.size() < uint32_t(-1)>>8);
			uint16_t childOffset = (uint16_t)childrenStack.size();
			uint8_t validMask = layers[layerNdx][parentPos];
			nodes.push_back(int32_t(childOffset) << 8 | validMask);

			// Don't add leafs
			if(layerNdx == treeDepth-1)
				continue;

			// Push children
			int32_t childSideSize = 2<<layerNdx;
			for(uint8_t child = 0; child < 8; ++child)
			{
				if(validMask & (1<<child))
				{
					int cx = 2*x+(child>>2);
					int cy = 2*y+((child>>1)&1);
					int cz = 2*z+(child&1);
					int childPos = ((cx*childSideSize)+cy)*childSideSize+cz;
					childrenStack.push_back({layerNdx+1, childPos});
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
					float x = float(i)/(sideSize-1);
					float z = float(k)/(sideSize-1);
					float h = (sideSize-1)*(sin(10*x)*0.025+0.5 + sin(16*z)*0.01);
					dst.voxels[index] = j <= h? 1 : 0;
				}
			}
		}
	}
}