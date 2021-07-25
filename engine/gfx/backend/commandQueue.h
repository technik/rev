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

#include "Vulkan/Vulkan.h"
#include <queue>

namespace rev::gfx
{
    class CommandQueue
    {
    public:
        CommandQueue(
            // Queue family
        );
        ~CommandQueue();

        // Disable copy
        CommandQueue(const CommandQueue&) = delete;
        void operator=(const CommandQueue&) = delete;

        // Queue family
        uint32_t family() const { return m_queueFamily; }

        // Get a new command buffer
        vk::CommandBuffer getCommandBuffer();
        // Get a new scoped command buffer
        vk::CommandBuffer getScopedCommandBuffer(vk::Semaphore waitForSemaphore = vk::Semaphore());

        // Submit a command buffer to the queue, and return a completion semaphore value
        uint64_t submit(vk::CommandBuffer);

        // Wait for timeline value
        void waitFor(uint64_t timeline);
        
        // check timeline value to see if all work submitted before the token is complete
        bool isComplete(uint64_t timelineToken);
        
        // flush: wait for completion of all sumbitted work
        void flush();

    private:
        // Command queue
        vk::Queue m_vkQueue;
        uint32_t m_queueFamily;

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