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

namespace rev::gfx
{
    CommandQueueDX12::CommandQueueDX12(ID3D12Device2& device, Info info)
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

        switch (info.priority)
        {
        case Priority::RealTime:
            desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_GLOBAL_REALTIME;
            break;
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
    }
}