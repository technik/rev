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

namespace rev::gfx
{
	class VulkanAllocator;

	class GPUBuffer
	{
	public:
		bool empty() const { return size() == 0; }

		vk::Buffer buffer() const { return m_buffer; }
		vk::DeviceMemory memory() const { return m_memory; }
		size_t offset() const { return m_offset; } // Offset from the memory allocation start
		size_t size() const { return m_size; } // Buffer size in bytes

		// Disable copy
		GPUBuffer() = default;
		GPUBuffer(const GPUBuffer&) = delete;

	private:
		GPUBuffer(vk::Buffer buffer, vk::DeviceMemory memory, size_t offset, size_t size)
			: m_buffer(buffer), m_memory(memory), m_offset(offset), m_size(size)
		{}

		friend class VulkanAllocator;

		vk::Buffer m_buffer;
		vk::DeviceMemory m_memory;
		size_t m_offset{}; // Offset from the memory allocation start
		size_t m_size{}; // Buffer size
	};
}