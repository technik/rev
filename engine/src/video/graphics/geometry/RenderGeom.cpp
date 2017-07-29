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
	RenderGeom::RenderGeom () {
		// Static data for a triangle
		const vector<Vec3f> rawVertexBuffer = {
			Vec3f(0.0f, 0.0f, 1.f), Vec3f(141.f/255.0f, 252.0f/255.0f, 247.0f/255.0f),
			Vec3f(1.f, 0.0f, -1.f), Vec3f(141.f/255.0f, 252.0f/255.0f, 247.0f/255.0f),
			Vec3f(-1.f, 0.0f, -1.f), Vec3f(255.f/255.0f, 51.0f/255.0f, 153.0f/255.0f)
		};

		mNVerts = 3;

		VkDeviceSize bufferSizeInBytes = rawVertexBuffer.size()*sizeof(Vec3f);
		GraphicsDriver::get().createBuffer(bufferSizeInBytes, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mVertexBuffer, mVertexBufferMemory);

		VkDevice device = GraphicsDriver::get().device();
		void* gpuMem;
		vkMapMemory(device, mVertexBufferMemory, 0, bufferSizeInBytes, 0, &gpuMem);
		memcpy(gpuMem, rawVertexBuffer.data(), (size_t)bufferSizeInBytes);
		vkUnmapMemory(device, mVertexBufferMemory);

		// Index buffer
		const std::vector<uint16_t> indices = {
			0, 1, 2
		};

		VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
		GraphicsDriver::get().createBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mIndexBuffer, mIndexBufferMemory);
		
		vkMapMemory(device, mIndexBufferMemory, 0, bufferSize, 0, &gpuMem);
		memcpy(gpuMem, indices.data(), (size_t)bufferSize);
		vkUnmapMemory(device, mIndexBufferMemory);
	}
}}