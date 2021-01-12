//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2021 Carmelo J Fdez-Aguera
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#pragma once

#include "vulkanAllocator.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

namespace rev::gfx {

	auto VulkanAllocator::createBuffer(size_t size, vk::BufferUsageFlags usage) -> GPUBuffer
	{
		vk::BufferCreateInfo bufferInfo({},size,usage,vk::SharingMode::eExclusive);

		auto buffer = m_device.createBuffer(bufferInfo);
		if (!buffer)
			return {};
		
		vk::MemoryRequirements memRequirements = m_device.getBufferMemoryRequirements(buffer);

		vk::MemoryPropertyFlags properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent; // So that we can map to it
		auto memType = findMemoryType(memRequirements.memoryTypeBits, properties);
		vk::MemoryAllocateInfo allocInfo(memRequirements.size, memType);

		auto memory = m_device.allocateMemory(allocInfo);
		if (!memory)
		{
			m_device.destroyBuffer(buffer);
			return {};
		}
		m_device.bindBufferMemory(buffer, memory, 0);

		return GPUBuffer(buffer, memory, 0, size);
	}

	void VulkanAllocator::destroyBuffer(const GPUBuffer& buffer)
	{
		m_device.freeMemory(buffer.m_memory);
		m_device.destroyBuffer(buffer.m_buffer);
	}

	void* VulkanAllocator::mapBufferInternal(const GPUBuffer& _buffer)
	{
		auto devMemory = m_device.mapMemory(_buffer.memory(), _buffer.offset(), _buffer.size());
		m_mappedMemory.insert({ devMemory, _buffer.memory() });
		return devMemory;
	}

	void VulkanAllocator::unmapBufferInternal(void* _buffer)
	{
		auto iter = m_mappedMemory.find(_buffer);
		assert(iter != m_mappedMemory.end());

		m_device.unmapMemory(iter->second);
		m_mappedMemory.erase(iter);
	}

	uint32_t VulkanAllocator::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
	{
		auto memProperties = m_physicalDevice.getMemoryProperties();
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if (typeFilter & (1 << i)) {
				return i;
			}
		}
		assert(false && "Unable to find a suitable memory type for allocation");
		return uint32_t(-1);
	}
}