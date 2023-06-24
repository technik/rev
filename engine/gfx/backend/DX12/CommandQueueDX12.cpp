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
#include "CommandQueueDX12.h"

#include "dx12Util.h"
#include <cassert>

namespace rev::gfx
{
    CommandQueueDX12::CommandQueueDX12(ID3D12Device2& device, Info info)
        : m_device(device)
    {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        switch (info.type)
        {
        case Type::Graphics:
            desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            break;
        case Type::Compute:
            desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
            break;
        case Type::Copy:
            desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
            break;
        }
        m_CommandListType = desc.Type;

        switch (info.priority)
        {
        case Priority::High:
            desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
            break;
        case Priority::Normal:
            desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
            break;
        }

        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 0;

        ThrowIfFailed(device.CreateCommandQueue(&desc, IID_PPV_ARGS(&m_d3d12CommandQueue)));

        // Create the fence
        m_completedFenceValue = 0;
        m_nextFenceValue = 1;
        ThrowIfFailed(device.CreateFence(m_completedFenceValue, D3D12_FENCE_FLAG_SHARED, IID_PPV_ARGS(&m_d3d12Fence)));
    }

    CommandQueueDX12::~CommandQueueDX12()
    {
        refreshInFlightWork();

        assert(m_inFlightCmdLists.empty());
    }


    bool CommandQueueDX12::isFenceComplete(uint64_t fenceValue)
    {
        if (fenceValue <= m_completedFenceValue)
            return true; // Early out

        refreshInFlightWork();
        return fenceValue <= m_completedFenceValue;
    }

    uint64_t CommandQueueDX12::submitCommandList(CommandList& list)
    {
        auto& cmdList12 = static_cast<CommandListDX12&>(list);
        cmdList12.m_commandList->Close();

        ID3D12CommandList* const ppCommandLists[] = {
            cmdList12.m_commandList.Get()
        };

        m_d3d12CommandQueue->ExecuteCommandLists(1, ppCommandLists);
        uint64_t fenceValue = m_nextFenceValue++;
        m_d3d12CommandQueue->Signal(m_d3d12Fence.Get(), fenceValue);

        return fenceValue;
    }

    CommandList& CommandQueueDX12::getCommandList()
    {
        std::unique_ptr<CommandListDX12> cmdList12;

        if (m_freeCmdLists.empty())
        {
            refreshInFlightWork();
        }

        // Any free cmd lists available?
        if (!m_freeCmdLists.empty())
        {
            cmdList12 = std::move(m_freeCmdLists.back());
            m_freeCmdLists.pop_back();
        }
        else
        {
            // Allocate a new cmdList
            cmdList12 = createCommandList();
        }

        m_inFlightCmdLists.push_back(std::move(cmdList12));
        return *m_inFlightCmdLists.back();
    }

    std::unique_ptr<CommandListDX12> CommandQueueDX12::createCommandList()
    {
        ComPtr<ID3D12CommandAllocator> allocator;
        ThrowIfFailed(m_device.CreateCommandAllocator(m_CommandListType, IID_PPV_ARGS(&allocator)));

        ComPtr<ID3D12GraphicsCommandList2> commandList;
        ThrowIfFailed(m_device.CreateCommandList(0, m_CommandListType, allocator.Get(), nullptr, IID_PPV_ARGS(&commandList)));

        return std::make_unique<CommandListDX12>(commandList, allocator);
    }

    void CommandQueueDX12::refreshInFlightWork()
    {
        // Early out if cmd lists are already available
        if (!m_freeCmdLists.empty())
            return;

        // Test which cmd lists have finished since we last checked
        m_completedFenceValue = m_d3d12Fence->GetCompletedValue();
        for (auto riter = m_inFlightCmdLists.rbegin(); riter != m_inFlightCmdLists.rend(); ++riter)
        {
            auto& cmdLst = *riter;
            if (cmdLst->m_submissionFenceId <= m_completedFenceValue) // work is done
            {
                cmdLst->m_submissionFenceId = 0; // reset submission

                // Need to reset the native cmdlist here?
                ThrowIfFailed(cmdLst->m_commandList->Reset(cmdLst->m_allocator.Get(), nullptr));

                m_freeCmdLists.push_back(std::move(cmdLst));
                std::swap(cmdLst, m_inFlightCmdLists.back());
                m_inFlightCmdLists.pop_back();
            }
        }
    }
}