//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

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
			Vec3f(0.0f, 0.0f, 0.5f), Vec3f(141.f/255.0f, 252.0f/255.0f, 247.0f/255.0f),
			Vec3f(-0.5f, 0.0f, -0.5f), Vec3f(141.f/255.0f, 252.0f/255.0f, 247.0f/255.0f),
			Vec3f(0.5f, 0.0f, -0.5f), Vec3f(255.f/255.0f, 51.0f/255.0f, 153.0f/255.0f)
		};

		mNVerts = 3;

		vk::DeviceSize bufferSizeInBytes = rawVertexBuffer.size()*sizeof(Vec3f);
		GraphicsDriver::get().createBuffer(bufferSizeInBytes, vk::BufferUsageFlagBits::eVertexBuffer, 
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, mVertexBuffer, mVertexBufferMemory);

		vk::Device device(GraphicsDriver::get().device());
		void* gpuMem = device.mapMemory(mVertexBufferMemory, 0, bufferSizeInBytes);
		memcpy(gpuMem, rawVertexBuffer.data(), (size_t)bufferSizeInBytes);
		device.unmapMemory(mVertexBufferMemory);

		// Index buffer
		const std::vector<uint16_t> indices = {
			0, 1, 2
		};

		VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
		GraphicsDriver::get().createBuffer(bufferSize, vk::BufferUsageFlagBits::eIndexBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, mIndexBuffer, mIndexBufferMemory);

		gpuMem = device.mapMemory(mIndexBufferMemory, 0, bufferSize);
		memcpy(gpuMem, indices.data(), (size_t) bufferSize);
		device.unmapMemory(mIndexBufferMemory);
	}
}}