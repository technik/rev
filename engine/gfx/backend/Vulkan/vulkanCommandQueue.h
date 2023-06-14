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

#include "Vulkan.h"
#include "../CommandQueue.h"
#include <queue>

namespace rev::gfx
{
    class VulkanCommandQueue : public CommandQueue
    {
    public:
        VulkanCommandQueue(
            vk::Queue nativeQueue
        );
        ~VulkanCommandQueue() {}

        auto nativeQueue() const { return m_vkQueue; }

        // ---- Synchronization ----
        /// \return the fence value the CPU must wait for to reach this sync point.
        uint64_t signalFence(Fence&) override;

        // Run commands
        void executeCommandList(CommandList& list) override;

    private:
        // Command queue
        vk::Queue m_vkQueue;

        // Command allocator/pool
        struct CommandSubmission
        {
            uint64_t finishTime;
        };

        // Note: Command pools are GPU side allocators, and so can not be reused until execution of finishes of
        // all command buffers that allocate from it. Command buffers on the other side, are CPU side and can be
        // reused immediately after sumbission. In order to simplify usage of the command queue, we only expose
        // command buffers to the user, and keep a 1:1 mapping with command pools until submission. Once
        // Submitted, the command buffer will immediately be available in the queue again, but the associated
        // command pool will remain bloked until GPU side execution completes.
        std::queue<vk::CommandBuffer> m_commandBufferQueue; // This should really be a ring buffer
        std::queue<vk::CommandPool> m_commandPoolQueue; // This should also be a ring buffer
        std::vector<std::pair<vk::CommandBuffer, vk::CommandPool>> m_bufferToPoolMap;

        // Synchronization semaphore
        vk::Semaphore m_timeline;
        uint64_t m_lastSignaledValue = 0;
        uint64_t m_lastSubmisionValue = 0;
    };
}