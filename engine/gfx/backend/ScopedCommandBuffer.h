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

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

namespace rev::gfx {

	class ScopedCommandBuffer
	{
	public:
		ScopedCommandBuffer(vk::CommandBuffer c, vk::Queue queue, vk::Semaphore waitForSemaphore = vk::Semaphore())
			: cmd(c)
			, m_submitQueue(queue)
			, m_waitSemaphore(waitForSemaphore)
		{
			cmd.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
		}

		ScopedCommandBuffer(ScopedCommandBuffer&& other)
			: cmd(other.cmd)
		{
			other.cmd = vk::CommandBuffer{}; // Clear other
		}

		void operator=(ScopedCommandBuffer&& other)
		{
			cmd = other.cmd;
			other.cmd = vk::CommandBuffer{}; // Clear other
		}

		~ScopedCommandBuffer()
		{
			if (cmd)
			{
				cmd.end();

				uint32_t waitSemaphoreCount = 0;
				vk::Semaphore* waitSemaphores = nullptr;
				vk::PipelineStageFlags waitStages = {};

				if (m_waitSemaphore)
				{
					waitSemaphoreCount = 1;
					waitSemaphores = &m_waitSemaphore;

					waitStages |= vk::PipelineStageFlagBits::eColorAttachmentOutput;
				}

				vk::SubmitInfo submitInfo(
					waitSemaphoreCount, waitSemaphores, &waitStages, // wait
					1, &cmd, // commands
					0, nullptr); // signal
				m_submitQueue.submit(submitInfo);
			}
		}

		// Disable copy
		ScopedCommandBuffer(const ScopedCommandBuffer&) = delete;
		ScopedCommandBuffer& operator=(const ScopedCommandBuffer&) = delete;

		vk::CommandBuffer cmd;
		vk::Queue m_submitQueue;
		vk::Semaphore m_waitSemaphore;
	};

}