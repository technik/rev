//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2019 Carmelo J Fdez-Aguera
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
#include "commandQueueDX12.h"
#include "commandListDX12.h"
#include "fenceDX12.h"

namespace rev::gfx {

	//-----------------------------------------------------------------------------
	uint64_t CommandQueueDX12::signalFence(Fence& fence)
	{
		auto dx12Fence = static_cast<FenceDX12&>(fence);
		uint64_t fenceValueForSignal = ++dx12Fence.m_lastValue;

		m_dx12Queue->Signal(dx12Fence.m_dx12Fence.Get(), fenceValueForSignal);

		return fenceValueForSignal;
	}

	//-----------------------------------------------------------------------------
	void CommandQueueDX12::executeCommandList(CommandList& list)
	{
		auto& dx12CmdList = static_cast<CommandListDX12&>(list);
		ID3D12CommandList* lst = dx12CmdList.m_commandList.Get();
		m_dx12Queue->ExecuteCommandLists(1, &lst);
	}
}

