//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "RenderGeom.h"
#include <math/algebra/vector.h>
#include <vector>
#include <video/graphics/driver/graphicsDriver.h>

using namespace rev::math;
using namespace std;

namespace rev {
namespace video {

	//----------------------------------------------------------------------------------------------------------------------
	// Basic element of geometry that can be used for rendering.
	RenderGeom::RenderGeom (const VertexFormat& _fmt, uint32_t _nVertices, const void* _vtxData, uint32_t _nIndices, const uint16_t* _indices)
		: mVtxFormat(_fmt)
		, mNVertices(_nVertices)
		, mVertexData(_vtxData)
		, mNIndices(_nIndices)
		, mIndices(_indices)
	{}

	//----------------------------------------------------------------------------------------------------------------------
	#ifdef REV_USE_VULKAN
	void RenderGeom::sendBuffersToGPU() {
		// Send vertex buffer to gpu
		VkDeviceSize bufferSizeInBytes = mNVertices*mVtxFormat.stride();
		GraphicsDriver::get().createBuffer(bufferSizeInBytes, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mVertexBuffer, mVertexBufferMemory);

		VkDevice device = GraphicsDriver::get().device();
		void* gpuMem;
		vkMapMemory(device, mVertexBufferMemory, 0, bufferSizeInBytes, 0, &gpuMem);
		memcpy(gpuMem, mVertexData, (size_t)bufferSizeInBytes);
		vkUnmapMemory(device, mVertexBufferMemory);

		
		// Send Index buffer to gpu
		VkDeviceSize bufferSize = sizeof(mIndices[0]) * mNIndices;
		GraphicsDriver::get().createBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mIndexBuffer, mIndexBufferMemory);
		
		vkMapMemory(device, mIndexBufferMemory, 0, bufferSize, 0, &gpuMem);
		memcpy(gpuMem, mIndices, (size_t)bufferSize);
		vkUnmapMemory(device, mIndexBufferMemory);
	}
	#endif
}}

