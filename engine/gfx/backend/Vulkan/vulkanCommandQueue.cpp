//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2023 Carmelo J Fdez-Aguera
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
#include "vulkanCommandQueue.h"

namespace rev::gfx
{
    //-----------------------------------------------------------------------------------------------
    VulkanCommandQueue::VulkanCommandQueue(
        vk::Device device,
        uint32_t familyIndex
    )
        : m_device(device)
        , m_familyIndex(familyIndex)
    {
        m_vkQueue = m_device.getQueue(familyIndex, 0);
    }

    //-----------------------------------------------------------------------------------------------
    VulkanCommandQueue::~VulkanCommandQueue()
    {
        assert(m_inFlightCmdLists.empty());
        for(auto& cmdLst : m_freeCmdLists)
        {
            // TODO: destroy the cmd buffers and pools
            m_device.destroyFence(cmdLst->m_fence);
        }
    }

    //-----------------------------------------------------------------------------------------------
    bool VulkanCommandQueue::isFenceComplete(uint64_t fenceValue)
    {
        if (fenceValue <= m_completedFenceValue)
            return true; // Early out

        refreshInFlightWork();
        return fenceValue <= m_completedFenceValue;
    }

    void VulkanCommandQueue::refreshInFlightWork()
    {
        // Early out if cmd lists are already available
        if (!m_freeCmdLists.empty())
            return;

        // Test which cmd lists have finished since we last checked
        for (auto riter = m_inFlightCmdLists.rbegin(); riter != m_inFlightCmdLists.rend(); ++riter)
        {
            auto& cmdLst = *riter;
            if (m_completedFenceValue > cmdLst->m_submissionFenceId || // Early out to avoid querying the API
                m_device.getFenceStatus(cmdLst->m_fence) == vk::Result::eSuccess) // work is done
            {
                m_completedFenceValue = std::max(m_completedFenceValue, cmdLst->m_submissionFenceId);
                cmdLst->m_submissionFenceId = 0; // reset submission
                m_device.resetFences(cmdLst->m_fence); // reset fence

                // Reset the native cmdlist
                cmdLst->m_commandList.reset();

                m_freeCmdLists.push_back(std::move(cmdLst));
                std::swap(cmdLst, m_inFlightCmdLists.back());
                m_inFlightCmdLists.pop_back();
            }
        }
    }

    //-----------------------------------------------------------------------------------------------
    CommandList& VulkanCommandQueue::getCommandList()
    {
        std::unique_ptr<VulkanCommandList> cmdList;

        if (m_freeCmdLists.empty())
        {
            refreshInFlightWork();
        }

        // Any free cmd lists available?
        if (!m_freeCmdLists.empty())
        {
            cmdList = std::move(m_freeCmdLists.back());
            m_freeCmdLists.pop_back();
        }
        else
        {
            // Allocate a new cmdList
            cmdList = createCommandList();
        }

        m_inFlightCmdLists.push_back(std::move(cmdList));
        return *m_inFlightCmdLists.back();
    }

    //-----------------------------------------------------------------------------------------------
    uint64_t VulkanCommandQueue::submitCommandList(CommandList& list)
    {
        auto& cmdListVk = static_cast<VulkanCommandList&>(list);
        cmdListVk.m_commandList.end();

        cmdListVk.m_submissionFenceId = m_nextFenceValue++;
        vk::SubmitInfo submitDesc;
        submitDesc.setCommandBuffers(cmdListVk.m_commandList);
        submitDesc.setSignalSemaphores({});
        submitDesc.setWaitSemaphores({});

        m_vkQueue.submit(submitDesc, cmdListVk.m_fence);

        return cmdListVk.m_submissionFenceId;
    }

    //-----------------------------------------------------------------------------------------------
    std::unique_ptr<VulkanCommandList> VulkanCommandQueue::createCommandList()
    {
        vk::CommandPoolCreateInfo poolDesc;
        poolDesc.queueFamilyIndex = m_familyIndex;
        vk::CommandPool allocator = m_device.createCommandPool(poolDesc);

        vk::CommandBufferAllocateInfo cmdListDesc;
        cmdListDesc.commandBufferCount = 1;
        cmdListDesc.commandPool = allocator;
        cmdListDesc.level = vk::CommandBufferLevel::ePrimary;

        vk::CommandBuffer cmdLst = m_device.allocateCommandBuffers(cmdListDesc).front(); // Silly to use a std::vector here...

        return std::make_unique<VulkanCommandList>(cmdLst, allocator);
    }

} // rev::gfx