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

namespace {
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(rev::video::GraphicsDriver::get().physicalDevice(), &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i))  && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		return uint32_t(-1);
	}
}

namespace rev {
namespace video {

	//----------------------------------------------------------------------------------------------------------------------
	// Basic element of geometry that can be used for rendering.
	RenderGeom::RenderGeom () {
		// Static data for a triangle
		const vector<Vec3f> rawVertexBuffer = {
			Vec3f(0.0f, -0.5f, 0.f), Vec3f(141.f/255.0f, 252.0f/255.0f, 247.0f/255.0f),
			Vec3f(0.5f, 0.5f, 0.f), Vec3f(141.f/255.0f, 252.0f/255.0f, 247.0f/255.0f),
			Vec3f(-0.5f, 0.5f, 0.f), Vec3f(255.f/255.0f, 51.0f/255.0f, 153.0f/255.0f)
		};

		vk::BufferCreateInfo createInfo = vk::BufferCreateInfo({}, // No flags
										rawVertexBuffer.size()*sizeof(Vec3f), // Size in bytes
										vk::BufferUsageFlagBits::eVertexBuffer, // Usage
										vk::SharingMode::eExclusive); // Sharing mode between queue families
		vk::Device device(GraphicsDriver::get().device());
		mVertexBuffer = device.createBuffer(createInfo);

		vk::MemoryRequirements memRequirements;
		device.getBufferMemoryRequirements(mVertexBuffer, &memRequirements);

		vk::MemoryAllocateInfo allocInfo = vk::MemoryAllocateInfo(memRequirements.size, 
			findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));

		mVertexBufferMemory = device.allocateMemory(allocInfo);
	}
}}