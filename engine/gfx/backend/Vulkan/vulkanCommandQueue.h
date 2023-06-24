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
    class VulkanCommandList : public CommandList
    {
    public:
        VulkanCommandList(vk::CommandBuffer nativeCmdList, vk::CommandPool allocator)
            : m_commandList(nativeCmdList)
            , m_allocator(allocator)
        {}

        vk::CommandBuffer m_commandList;
        vk::CommandPool m_allocator;
        vk::Fence m_fence;
        uint64_t m_submissionFenceId = 0; // 0 means not submitted
    };

    class VulkanCommandQueue : public CommandQueue
    {
    public:
        VulkanCommandQueue(
            vk::Device device,
            uint32_t familyIndex
        );
        ~VulkanCommandQueue();

        auto nativeQueue() const { return m_vkQueue; }

        // ---- Synchronization ----
        bool isFenceComplete(uint64_t fenceValue) override;
        void refreshInFlightWork() override;

        // Run commands
        CommandList& getCommandList() override;
        uint64_t submitCommandList(CommandList& list) override;

    private:
        std::vector<std::unique_ptr<VulkanCommandList>> m_freeCmdLists;
        std::vector<std::unique_ptr<VulkanCommandList>> m_inFlightCmdLists;

        std::unique_ptr<VulkanCommandList> createCommandList();

        vk::Device m_device;
        vk::Queue m_vkQueue;
        int32_t m_familyIndex;

        uint64_t    m_nextFenceValue = 1;
        uint64_t    m_completedFenceValue = 0;
    };
}