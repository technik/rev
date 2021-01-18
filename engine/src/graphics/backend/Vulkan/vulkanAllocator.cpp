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

using namespace std::chrono;
using namespace std::chrono_literals;

namespace rev::gfx {

	auto VulkanAllocator::createBufferInternal(size_t size, vk::BufferUsageFlags usage, MemoryAccess memoryAccess, const std::vector<uint32_t>& queueFamilies) -> std::shared_ptr<GPUBuffer>
	{
		vk::SharingMode bufferQueueSharing = (queueFamilies.size() > 1) ? vk::SharingMode::eConcurrent: vk::SharingMode::eExclusive;
		vk::BufferCreateInfo bufferInfo({},size,usage,bufferQueueSharing,queueFamilies);

		auto buffer = m_device.createBuffer(bufferInfo);
		if (!buffer)
			return {};
		
		vk::MemoryRequirements memRequirements = m_device.getBufferMemoryRequirements(buffer);

		vk::MemoryPropertyFlags properties;
		if(memoryAccess == MemoryAccess::host)
			properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent; // So that we can map to it
		else
			properties = vk::MemoryPropertyFlagBits::eDeviceLocal; // So that we can map to it
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

	auto VulkanAllocator::createGpuBuffer(size_t size, vk::BufferUsageFlags usage, uint32_t graphicsQueueFamily)->std::shared_ptr<GPUBuffer>
	{
		std::vector<uint32_t> queueFamilies = { graphicsQueueFamily };
		bool isTransferDst = (usage & vk::BufferUsageFlagBits::eTransferDst) == vk::BufferUsageFlagBits::eTransferDst;
		if (isTransferDst && (graphicsQueueFamily != m_transferQueueFamily))
			queueFamilies.push_back(m_transferQueueFamily);

		return createBufferInternal(size, usage, MemoryAccess::device, queueFamilies);
	}

	auto VulkanAllocator::createSharedBuffer(size_t size, vk::BufferUsageFlags usage, uint32_t graphicsQueueFamily)->std::shared_ptr<GPUBuffer>
	{
		std::vector<uint32_t> queueFamilies = { graphicsQueueFamily };
		return createBufferInternal(size, usage, MemoryAccess::host, queueFamilies);
	}

	void VulkanAllocator::destroyBuffer(const GPUBuffer& buffer)
	{
		m_device.freeMemory(buffer.m_memory);
		m_device.destroyBuffer(buffer.m_buffer);
	}

	void VulkanAllocator::resizeStreamingBuffer(size_t minSize)
	{
		if (m_capacity >= minSize)
			return; // Early out if we already have sufficient capacity

		m_streamingQueue.waitIdle();
		m_stagingBuffer = createBufferInternal(minSize, vk::BufferUsageFlagBits::eTransferSrc, MemoryAccess::host, { m_transferQueueFamily });
		m_capacity = m_stagingBuffer->size();

		// Reset counters
		m_freeFences.reserve(m_pendingBlocks.size() + m_freeFences.size());
		for (auto& block : m_pendingBlocks)
		{
			m_device.resetFences(block.fence);
			m_freeFences.push_back(block.fence);
		}
		m_pendingBlocks.clear();

		m_ringReadPos = m_capacity;
		m_ringWritePos = 0;
	}

	size_t VulkanAllocator::asyncTransferInternal(const GPUBuffer& dst, const uint8_t* src, size_t size, size_t dstOffset)
	{
		// Request too big? assert
		assert(size <= m_stagingBuffer->size());

		advanceReadPos();

		// Ring exhausted?
		while (availableSpace() < size) // Can't write exactly as much memory as available, because readPos==writePos means the buffer is empty
		{
			m_device.waitForFences(m_pendingBlocks.front().fence, 1, duration_cast<nanoseconds>(1ms).count());
			advanceReadPos();
		}

		// Write 0 to end of buffer
		auto writePos = m_ringWritePos % m_stagingBuffer->size();
		auto sizeToEnd = m_stagingBuffer->size() - writePos;
		auto writeSize = std::min(sizeToEnd, size);

		writeToRingBuffer(dst, 0, src, writeSize);
		size -= writeSize;

		if (size > 0)
		{
			src += writeSize;
			writeToRingBuffer(dst, writeSize, src, size);
		}

		return m_ringWritePos;
	}

	void VulkanAllocator::writeToRingBuffer(const GPUBuffer& dst, size_t dstOffset, const void* src, size_t size)
	{
		auto writePos = m_ringWritePos % m_stagingBuffer->size();
		copyToGPUInternal(*m_stagingBuffer, writePos, src, size);
		m_ringWritePos += size;

		InFlightBlock writeBlock;
		writeBlock.fence = getFreeFence();
		writeBlock.size = size;
		vk::CommandBufferAllocateInfo cmdInfo(m_transferPool, vk::CommandBufferLevel::ePrimary, 1);
		writeBlock.cmd = m_device.allocateCommandBuffers(cmdInfo).front();
		
		writeBlock.cmd.begin({ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
		vk::BufferCopy region;
		region.dstOffset = dst.offset() + dstOffset;
		region.srcOffset = writePos + m_stagingBuffer->offset();
		region.size = size;
		writeBlock.cmd.copyBuffer(m_stagingBuffer->buffer(), dst.buffer(), region);
		writeBlock.cmd.end();

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
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties))
			{
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
			m_device.resetFences(block.fence);
			m_freeFences.push_back(block.fence);
			// Clear the block
			m_device.freeCommandBuffers(m_transferPool, block.cmd);
			++clearedblocks;
			// Mark memory as read
			m_ringReadPos += block.size;
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