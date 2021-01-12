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

#include <chrono>
#include <thread>

using namespace std::chrono_literals;

namespace rev::gfx {

	auto VulkanAllocator::createBuffer(size_t size, vk::BufferUsageFlags usage) -> std::shared_ptr<GPUBuffer>
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

		return std::shared_ptr<GPUBuffer>(new GPUBuffer(buffer, memory, 0, size), 
			[this](GPUBuffer* p)
			{
				destroyBuffer(*p);
				delete p;
			});
	}

	void VulkanAllocator::destroyBuffer(const GPUBuffer& buffer)
	{
		m_device.freeMemory(buffer.m_memory);
		m_device.destroyBuffer(buffer.m_buffer);
	}

	size_t VulkanAllocator::asyncTransfer(const GPUBuffer& dst, uint8_t* src, size_t size, size_t dstOffset)
	{
		// Request too big? assert
		assert(size <= m_stagingBuffer->size());

		advanceReadPos();

		// Ring exhausted?
		while (availableSpace() < size) // Can't write exactly as much memory as available, because readPos==writePos means the buffer is empty
		{
			std::this_thread::sleep_for(1ms);
			advanceReadPos();
		}

		// Write 0 to end of buffer
		auto writePos = m_ringWritePos % m_stagingBuffer->size();
		auto sizeToEnd = m_stagingBuffer->size() - writePos;
		auto writeSize = std::min(sizeToEnd, size);

		writeToRingBuffer(src, writeSize);
		size -= writeSize;

		if (size > 0)
		{
			src += writeSize;
			writeToRingBuffer(src, size);
		}

		return m_ringWritePos;
	}

	void VulkanAllocator::writeToRingBuffer(void* src, size_t size)
	{
		auto writePos = m_ringWritePos % m_stagingBuffer->size();
		copyToGPUInternal(*m_stagingBuffer, writePos, src, size);
		m_ringWritePos += size;

		InFlightBlock writeBlock;
		writeBlock.fence = getFreeFence();
		writeBlock.size = size;
		vk::CommandBufferAllocateInfo cmdInfo(m_transferPool, vk::CommandBufferLevel::ePrimary, 1);
		writeBlock.cmd = m_device.allocateCommandBuffers(cmdInfo).front();
		vk::SubmitInfo submitInfo(
			0, nullptr, nullptr, // Wait on
			1, &writeBlock.cmd, // commands
			0, nullptr); // signal)
		m_streamingQueue.submit(submitInfo, writeBlock.fence);

		m_pendingBlocks.push_back(writeBlock);
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

	void VulkanAllocator::copyToGPUInternal(const GPUBuffer& dst, size_t dstOffset, const void* src, size_t size)
	{
		if (!size)
			return; // Early out
		assert(size + dstOffset <= dst.size());
		auto devMemory = m_device.mapMemory(dst.memory(), dst.offset() + dstOffset, size);
		memcpy(devMemory, src, size);
		m_device.unmapMemory(dst.memory());
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

	void VulkanAllocator::advanceReadPos()
	{
		size_t clearedblocks = 0;
		for (auto& block : m_pendingBlocks)
		{
			if (m_device.getFenceStatus(block.fence) == vk::Result::eNotReady)
				break;

			// Clear the fence for reuse
			m_freeFences.push_back(block.fence);
			// Release memory in the ring
			m_ringReadPos += block.size;
			m_device.freeCommandBuffers(m_transferPool, block.cmd);
			++clearedblocks;
		}
		// Remove empty blocks from the queue
		auto remainingBlocks = m_pendingBlocks.size() - clearedblocks;
		for (size_t i = 0; i < remainingBlocks; ++i)
		{
			m_pendingBlocks[i] = m_pendingBlocks[i + clearedblocks];
		}
		m_pendingBlocks.resize(remainingBlocks);
	}

	vk::Fence VulkanAllocator::getFreeFence()
	{
		if (m_freeFences.empty())
			return m_device.createFence(vk::FenceCreateInfo({}));
		auto backFence = m_freeFences.back();
		m_freeFences.pop_back();
		return backFence;
	}
}