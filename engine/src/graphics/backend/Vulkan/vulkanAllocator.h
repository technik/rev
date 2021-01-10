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

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

namespace rev::gfx {

	// C++ Wrapper around the vulkan memory allocator
	class VulkanAllocator
	{
	public:
		VulkanAllocator() = default;
		VulkanAllocator(vk::Device device, vk::PhysicalDevice physicalDevice, vk::Instance instance)
			: m_device(device)
		{
			VmaAllocatorCreateInfo allocatorInfo = {};
			allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_1;
			allocatorInfo.physicalDevice = physicalDevice;
			allocatorInfo.device = device;
			allocatorInfo.instance = instance;

			vmaCreateAllocator(&allocatorInfo, &m_allocator);
		}

		vk::Buffer createBuffer(size_t size, vk::BufferUsageFlags usage, VmaMemoryUsage intent)
		{
			vk::BufferCreateInfo bufferInfo({}, size, usage);

			VmaAllocationCreateInfo allocInfo = {};
			allocInfo.usage = intent;

			VkBuffer buffer;
			VkBufferCreateInfo rawCreateInfo = bufferInfo;
			VmaAllocation allocation;
			vmaCreateBuffer(m_allocator, &rawCreateInfo, &allocInfo, &buffer, &allocation, nullptr);

			m_openAllocations.emplace_back(buffer, allocation); // So that we can destroy it later

			return vk::Buffer(buffer);
		}

		void destroyBuffer(vk::Buffer _buffer)
		{
			for(size_t i = 0; i < m_openAllocations.size(); ++i)
			{
				const auto& [buffer, alloc] = m_openAllocations[i];
				if (buffer == _buffer)
				{
					vmaDestroyBuffer(m_allocator, buffer, alloc);
					m_openAllocations[i] = m_openAllocations.back();
					m_openAllocations.pop_back();
					return;
				}
			}
			assert(false && "Buffer already freed, or never allocated");
		}

		template<class T>
		T* mapBuffer(vk::Buffer _buffer)
		{
			void* mappedMemory{};
			const auto& [buffer, alloc] = findBuffer(_buffer);
			
			//auto memory = alloc->GetMemory();
			//auto offset = alloc->GetOffset();
			//mappedMemory = m_device.mapMemory(memory, offset, sizeof(T) * count, {});

			auto result = vmaMapMemory(m_allocator, alloc, &mappedMemory);
			if (result != VK_SUCCESS)
				return nullptr;

			return (T*)mappedMemory;
		}

		template<class T>
		void unmapBuffer(T* _buffer)
		{
			auto iter = m_mappedMemory.find((void*)_buffer);
			assert(iter != m_mappedMemory.end());

			//auto memory = iter->second->GetMemory();
			//m_device.unmapMemory(memory);

			vmaUnmapMemory(m_allocator, iter->second);
		}

	private:
		vk::Device m_device;
		VmaAllocator m_allocator{ nullptr };

		const auto& findBuffer(vk::Buffer _buffer) const {
			for (size_t i = 0; i < m_openAllocations.size(); ++i)
			{
				const auto& [buffer, alloc] = m_openAllocations[i];
				if (buffer == _buffer)
				{
					return m_openAllocations[i];
				}
			}
			assert(false && "Can't find requested buffer allocation");
		}

		std::vector<std::pair<VkBuffer, VmaAllocation>> m_openAllocations;
		std::unordered_map<void*, VmaAllocation> m_mappedMemory;
	};
}