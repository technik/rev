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

#include "renderContextVulkan.h"
#include <gfx/Image.h>

#include <chrono>
#include <thread>

using namespace std::chrono;
using namespace std::chrono_literals;

namespace rev::gfx {

	vk::MemoryPropertyFlags VulkanAllocator::getVulkanMemoryProperties(MemoryProperties flags)
	{
		vk::MemoryPropertyFlags vulkanProperties;
		if (flags & MemoryProperties::supportsHostMapping)
		{
			vulkanProperties =
				vk::MemoryPropertyFlagBits::eHostVisible | // So that we can map to it
				vk::MemoryPropertyFlagBits::eHostCoherent; // So that we don't need to flush it after writing
		}
		if (flags & MemoryProperties::deviceLocal)
		{
			vulkanProperties |= vk::MemoryPropertyFlagBits::eDeviceLocal;
		}
		return vulkanProperties;
	}

	void VulkanAllocator::transitionImageLayout(vk::CommandBuffer cmd, vk::Image image, vk::Format imageFormat, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, bool isDepth)
	{
		vk::ImageMemoryBarrier barrier;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;

		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.aspectMask = isDepth ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor;

		vk::PipelineStageFlags srcStageMask, dstStageMask;
		if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) // Staging buffer
		{
			barrier.srcAccessMask = {};
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

			srcStageMask = vk::PipelineStageFlagBits::eTopOfPipe;
			dstStageMask = vk::PipelineStageFlagBits::eTransfer;
		}
		else if(oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) // Texture dst
		{
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

			srcStageMask = vk::PipelineStageFlagBits::eTransfer;
			dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;
		}
		else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eGeneral) // Attachments
		{
			srcStageMask = vk::PipelineStageFlagBits::eTopOfPipe;
			dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;

			barrier.srcAccessMask = {};
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite;
		}
		else if (oldLayout == vk::ImageLayout::eShaderReadOnlyOptimal && newLayout == vk::ImageLayout::eGeneral) // Attachments
		{
			srcStageMask = vk::PipelineStageFlagBits::eFragmentShader;
			dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;

			barrier.srcAccessMask = vk::AccessFlagBits::eShaderRead;
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderWrite;
		}
		else if (oldLayout == vk::ImageLayout::eGeneral && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) // Attachments
		{
			srcStageMask = vk::PipelineStageFlagBits::eFragmentShader;
			dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;

			barrier.srcAccessMask = vk::AccessFlagBits::eShaderWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
		}
		else
		{
			assert(false && "Unsupported layout transition");
		}

		cmd.pipelineBarrier(
			srcStageMask,
			dstStageMask,
			{},
			{}, // Memory barriers
			{}, // Buffer mem
			barrier); // Image barriers
	}

	uint32_t VulkanAllocator::getVulkanMemoryHeap(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
	{
		auto memProperties = m_physicalDevice.getMemoryProperties();
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties))
			{
				return i;
			}
		}
		assert(false && "Unable to find a suitable memory heap for allocation");
		return uint32_t(-1);
	}

	uint32_t VulkanAllocator::getVulkanMemoryHeap(MemoryProperties flags)
	{
		auto vulkanMemoryProperties = getVulkanMemoryProperties(flags);
		return getVulkanMemoryHeap(~0,vulkanMemoryProperties);
	}

	auto VulkanAllocator::createBufferInternal(size_t size, vk::BufferUsageFlags usage, MemoryProperties memoryProperties, const std::vector<uint32_t>& queueFamilies) -> std::shared_ptr<GPUBuffer>
	{
		const vk::SharingMode bufferQueueSharing = (queueFamilies.size() > 1) ? vk::SharingMode::eConcurrent: vk::SharingMode::eExclusive;
		const vk::BufferCreateInfo bufferInfo({},size,usage,bufferQueueSharing,queueFamilies);

		// Create a buffer handle
		const auto buffer = m_device.createBuffer(bufferInfo);
		if (!buffer)
			return {};
		
		// Allocate memory for the buffer
		const auto memoryHeap = getVulkanMemoryHeap(memoryProperties);
		const auto memRequirements = m_device.getBufferMemoryRequirements(buffer);
		const vk::MemoryAllocateInfo allocInfo(memRequirements.size, memoryHeap);
		const auto memory = m_device.allocateMemory(allocInfo);

		if (!memory)
		{
			m_device.destroyBuffer(buffer);
			return {};
		}

		// Bind handle and memory
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

		return createBufferInternal(size, usage, MemoryProperties::deviceLocal, queueFamilies);
	}

	auto VulkanAllocator::createImageBuffer(
		const char* name,
		math::Vec2u size,
		vk::Format format,
		vk::ImageUsageFlags usage,
		uint32_t graphicsQueueFamily) ->std::shared_ptr<ImageBuffer>
	{
		return createImageBufferInternal(name, size, format, usage, graphicsQueueFamily, false);
	}

	auto VulkanAllocator::createDepthBuffer(
		const char* name,
		math::Vec2u size,
		vk::Format format,
		vk::ImageUsageFlags usage,
		uint32_t graphicsQueueFamily) ->std::shared_ptr<ImageBuffer>
	{
		return createImageBufferInternal(name, size, format, usage, graphicsQueueFamily, true);
	}

	auto VulkanAllocator::createTexture(
		RenderContextVulkan& rc,
		const char* debugName,
		const math::Vec2u& imageSize,
		vk::Format gpuFormat,
		vk::SamplerAddressMode repeatX,
		vk::SamplerAddressMode repeatY,
		bool anisotropy,
		size_t mipLevels,
		void* data,
		vk::ImageUsageFlags usage,
		uint32_t graphicsQueueFamily) ->std::shared_ptr<Texture>
	{

		// Create a staging buffer
		size_t bufferSize = GetPixelSize(gpuFormat) * imageSize.x() * imageSize.y();
		auto buffer = createBufferForMapping(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, graphicsQueueFamily);
		// Copy data to the buffer
		auto dataDst = mapBuffer<uint8_t*>(*buffer);
		memcpy(dataDst, data, bufferSize);
		unmapBuffer(dataDst);

		// Create the final texture image
		auto dstUsage = usage | vk::ImageUsageFlagBits::eTransferDst;
		auto image = createImageBufferInternal(debugName, imageSize, gpuFormat, dstUsage, graphicsQueueFamily, false);

		// Transition image to copyDst
		{
			auto scopedCmd = rc.getScopedCmdBuffer(rc.graphicsQueue());
			transitionImageLayout(scopedCmd.cmd, image->image(), gpuFormat, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, false);

			// Copy texture data to it through a staging buffer
			vk::BufferImageCopy region{};
			region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
			region.imageSubresource.mipLevel = 0;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = 1;
			region.imageExtent.width = imageSize.x();
			region.imageExtent.height = imageSize.y();
			region.imageExtent.depth = 1;
			scopedCmd.cmd.copyBufferToImage(buffer->buffer(), image->image(), vk::ImageLayout::eTransferDstOptimal, region);

			// Trasition the final image into general layout
			transitionImageLayout(scopedCmd.cmd, image->image(), gpuFormat, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, false);
		}
		m_device.waitIdle();

		// Image sampler
		vk::SamplerCreateInfo samplerInfo;
		samplerInfo.addressModeU = repeatX;
		samplerInfo.addressModeV = repeatY;
		samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
		samplerInfo.magFilter = vk::Filter::eLinear;
		samplerInfo.minFilter = vk::Filter::eLinear;
		samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
		if (anisotropy)
		{
			samplerInfo.anisotropyEnable = VK_TRUE;
			samplerInfo.maxAnisotropy = 16;
		}
		else
		{
			samplerInfo.anisotropyEnable = VK_FALSE;
			samplerInfo.maxAnisotropy = 0;
		}
		auto sampler = m_device.createSampler(samplerInfo);

		auto texture = std::make_shared<Texture>();
		// Take over the image and image view
		texture->image = std::move(image);
		texture->format = gpuFormat;
		texture->sampler = sampler;

		return texture;
	}

	auto VulkanAllocator::createBufferForMapping(size_t size, vk::BufferUsageFlags usage, uint32_t graphicsQueueFamily)->std::shared_ptr<GPUBuffer>
	{
		std::vector<uint32_t> queueFamilies = { graphicsQueueFamily };
		return createBufferInternal(size, usage, MemoryProperties::supportsHostMapping, queueFamilies);
	}

	void VulkanAllocator::destroyBuffer(const GPUBuffer& buffer)
	{
		m_device.freeMemory(buffer.m_memory);
		m_device.destroyBuffer(buffer.m_buffer);
	}

	void VulkanAllocator::reserveStreamingBuffer(size_t minSize)
	{
		if (m_capacity >= minSize)
			return; // Early out if we already have sufficient capacity

		m_streamingQueue.waitIdle();
		m_stagingBuffer = createBufferInternal(minSize, vk::BufferUsageFlagBits::eTransferSrc, MemoryProperties::supportsHostMapping, {});
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
		while (availableSpace() < size)
		{
			m_device.waitForFences(m_pendingBlocks.front().fence, 1, duration_cast<nanoseconds>(1ms).count());
			advanceReadPos();
		}

		// Write 0 to end of buffer
		auto writePos = m_ringWritePos % m_stagingBuffer->size();
		auto sizeToEnd = m_stagingBuffer->size() - writePos;
		auto writeSize = std::min(sizeToEnd, size);

		writeToRingBuffer(dst, dstOffset, src, writeSize);
		size -= writeSize;

		if (size > 0)
		{
			src += writeSize;
			writeToRingBuffer(dst, dstOffset + writeSize, src, size);
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

	std::shared_ptr<ImageBuffer> VulkanAllocator::createImageBufferInternal(
		const char* debugName,
		math::Vec2u size,
		vk::Format format,
		vk::ImageUsageFlags usage,
		uint32_t graphicsQueueFamily,
		bool isDepth)
	{
		std::vector<uint32_t> queueFamilies = { graphicsQueueFamily };
		bool isTransferDst = (usage & vk::ImageUsageFlagBits::eTransferDst) == vk::ImageUsageFlagBits::eTransferDst;
		
		if (isTransferDst && (graphicsQueueFamily != m_transferQueueFamily))
			queueFamilies.push_back(m_transferQueueFamily);

		vk::ImageCreateInfo imageInfo;
		imageInfo.imageType = vk::ImageType::e2D;
		imageInfo.extent.width = size.x();
		imageInfo.extent.height = size.y();
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;

		imageInfo.format = format;
		imageInfo.tiling = vk::ImageTiling::eOptimal;
		imageInfo.initialLayout = vk::ImageLayout::eUndefined;
		imageInfo.usage = usage;
		imageInfo.sharingMode = queueFamilies.size() > 1 ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive;
		imageInfo.setQueueFamilyIndices(queueFamilies);

		imageInfo.samples = vk::SampleCountFlagBits::e1;
		imageInfo.flags = {};

		auto vkImage = m_device.createImage(imageInfo);
		if (!vkImage)
			return nullptr;

		// Allocate memory for the image
		vk::MemoryRequirements memoryRequirements = m_device.getImageMemoryRequirements(vkImage);
		vk::MemoryAllocateInfo allocInfo;
		allocInfo.allocationSize = memoryRequirements.size;
		allocInfo.memoryTypeIndex = getVulkanMemoryHeap(MemoryProperties(memoryRequirements.memoryTypeBits));

		auto imageMemory = m_device.allocateMemory(allocInfo);
		if (!imageMemory)
			return nullptr;

		m_device.bindImageMemory(vkImage, imageMemory, 0);

		vk::ImageViewCreateInfo viewInfo({},
			vkImage,
			vk::ImageViewType::e2D,
			format,
			{ vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity },
			vk::ImageSubresourceRange( isDepth ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
		auto imageView = m_device.createImageView(viewInfo);

		return std::shared_ptr<ImageBuffer>(new ImageBuffer(vkImage, imageView, format),
			[this, imageMemory, vkImage](ImageBuffer* p)
			{
				if (p->image())
				{
					m_device.freeMemory(imageMemory);
					m_device.destroyImage(vkImage);
				}
				delete p;
			});
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

		m_device.flushMappedMemoryRanges(vk::MappedMemoryRange(iter->second, 0, VK_WHOLE_SIZE));
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