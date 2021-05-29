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

#include <vulkan/vulkan.hpp>

#include "gpuBuffer.h"
#include <gfx/Texture.h>
#include <math/algebra/vector.h>

namespace rev::gfx {

	class RenderContextVulkan;

	// C++ Wrapper around the vulkan memory allocator
	class VulkanAllocator
	{
	public:
		VulkanAllocator() = default;
		VulkanAllocator(vk::Device device, vk::PhysicalDevice physicalDevice, vk::Instance instance, vk::Queue streamingQueue, uint32_t copyQueueFamily)
			: m_device(device)
			, m_physicalDevice(physicalDevice)
			, m_streamingQueue(streamingQueue)
			, m_transferQueueFamily(copyQueueFamily)
		{
			vk::CommandPoolCreateInfo poolInfo(vk::CommandPoolCreateFlagBits::eTransient, copyQueueFamily);
			m_transferPool = device.createCommandPool(poolInfo);
		}

		~VulkanAllocator()
		{
			m_streamingQueue.waitIdle();
			m_stagingBuffer = nullptr;
			for (auto& block : m_pendingBlocks)
				m_device.destroyFence(block.fence);
			for (auto& fence : m_freeFences)
				m_device.destroyFence(fence);
		}

		std::shared_ptr<GPUBuffer> createGpuBuffer(size_t size, vk::BufferUsageFlags usage, uint32_t graphicsQueueFamily);
		std::shared_ptr<GPUBuffer> createBufferForMapping(size_t size, vk::BufferUsageFlags usage, uint32_t graphicsQueueFamily);
		std::shared_ptr<ImageBuffer> createImageBuffer(const char* name, math::Vec2u size, vk::Format format, vk::ImageUsageFlags usage, uint32_t graphicsQueueFamily);
		std::shared_ptr<ImageBuffer> createDepthBuffer(const char* name, math::Vec2u size, vk::Format format, vk::ImageUsageFlags usage, uint32_t graphicsQueueFamily);

		// If data is not null, its contents will be copied into the dst texture, and its format is expected to be the same as gpuFormat
		std::shared_ptr<Texture> createTexture(
			RenderContextVulkan& rc,
			const char* debugName,
			const math::Vec2u& size,
			vk::Format gpuFormat,
			vk::SamplerAddressMode repeatX,
			vk::SamplerAddressMode repeatY,
			bool anisotropy,
			size_t mipLevels,
			void* data,
			vk::ImageUsageFlags usage,
			uint32_t graphicsQueueFamily);

		void destroyBuffer(const GPUBuffer&);

		template<class T>
		T* mapBuffer(const GPUBuffer& _buffer)
		{
			return (T*)mapBufferInternal(_buffer);
		}

		void reserveStreamingBuffer(size_t minSize);

		// You are free to erase or override the src memory as soon as this call returns.
		template<class T>
		size_t asyncTransfer(const GPUBuffer& dst, const T* src, size_t count, size_t dstOffset = 0)
		{
			return asyncTransferInternal(dst, (const uint8_t*)src, sizeof(T) * count, dstOffset);
		}

		bool isTransferFinished(size_t token) {
			advanceReadPos();
			return token <= (m_ringReadPos-m_capacity);
		}

		template<class T>
		void unmapBuffer(T* _buffer)
		{
			unmapBufferInternal((void*)_buffer);
		}

		template<class T>
		void copyToGPU(const GPUBuffer& dst, const T* src, size_t count, size_t dstOffset = 0)
		{
			copyToGPUInternal(dst, dstOffset, (const void*)src, count * sizeof(T));
		}

		void transitionImageLayout(vk::CommandBuffer cmd, vk::Image image, vk::Format imageFormat, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, bool isDepth);

	private:
		enum MemoryProperties
		{
			supportsHostMapping = 1,
			deviceLocal = 2
		};

		static vk::MemoryPropertyFlags getVulkanMemoryProperties(MemoryProperties flags);
		uint32_t getVulkanMemoryHeap(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
		uint32_t getVulkanMemoryHeap(MemoryProperties flags);

		std::shared_ptr<GPUBuffer> createBufferInternal(size_t size, vk::BufferUsageFlags usage, MemoryProperties memoryAccess, const std::vector<uint32_t>& queueFamilies);
		void* mapBufferInternal(const GPUBuffer& _buffer);
		void unmapBufferInternal(void*);
		void copyToGPUInternal(const GPUBuffer& dst, size_t dstOffset, const void* src, size_t count);
		void writeToRingBuffer(const GPUBuffer& dst, size_t dstOffset, const void* src, size_t size);
		std::shared_ptr<ImageBuffer> createImageBufferInternal(
			const char* debugName,
			math::Vec2u size,
			vk::Format format,
			vk::ImageUsageFlags usage,
			uint32_t graphicsQueueFamily,
			bool isDepth);


		vk::Device m_device;
		vk::PhysicalDevice m_physicalDevice;
		vk::Queue m_streamingQueue;
		uint32_t m_transferQueueFamily;
		
		std::unordered_map<void*, vk::DeviceMemory> m_mappedMemory;

		// Streaming
		size_t asyncTransferInternal(const GPUBuffer& dst, const uint8_t* src, size_t size, size_t dstOffset);
		size_t availableSpace() const
		{
			return (m_ringReadPos - m_ringWritePos);
		}
		void advanceReadPos();
		vk::Fence getFreeFence();

		struct InFlightBlock
		{
			vk::Fence fence;
			size_t size{};
			vk::CommandBuffer cmd;
		};

		vk::CommandPool m_transferPool;
		size_t m_ringReadPos{}, m_ringWritePos{}, m_capacity{};
		std::vector<vk::Fence> m_freeFences;
		std::vector<InFlightBlock> m_pendingBlocks;
		std::shared_ptr<GPUBuffer> m_stagingBuffer;
	};
}