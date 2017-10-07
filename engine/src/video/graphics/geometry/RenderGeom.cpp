//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "RenderGeom.h"
#include <core/platform/fileSystem/fileSystem.h>
#include <math/algebra/vector.h>
#include <vector>
#include <video/graphics/driver/graphicsDriver.h>
#include <fstream>

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
	RenderGeom* RenderGeom::loadFromFile(const std::string& _fileName) {
		std::ifstream file(core::FileSystem::get()->findPath(_fileName), std::ifstream::binary);
		if(!file.is_open())
			return nullptr;
		// Read header
		VertexFormat format;
		format.deserialize(file);
		uint16_t nVertices;
		uint32_t nIndices;
		file.read((char*)&nVertices, sizeof(nVertices));
		file.read((char*)&nIndices, sizeof(nIndices));
		// Read vertex data
		size_t vtxDataSize = format.stride()*nVertices;
		char* vertexData = new char[vtxDataSize];
		file.read(vertexData, vtxDataSize);
		// Read index data
		size_t idxDataSize = nIndices * sizeof(uint16_t);
		uint16_t* indices = new uint16_t[nIndices];
		file.read((char*)indices, idxDataSize);
		return new RenderGeom(format, nVertices, vertexData, nIndices, indices);
	}

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

