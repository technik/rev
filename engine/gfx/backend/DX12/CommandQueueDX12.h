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
#pragma once

#include "../CommandQueue.h"
#include "dx12Util.h"

namespace rev::gfx
{
	class CommandQueueDX12 : public CommandQueue
	{
	public:
		CommandQueueDX12(ID3D12Device2& device, Info type);

		// ---- Synchronization ----
		/// \return the fence value the CPU must wait for to reach this sync point.
		uint64_t signalFence(Fence&) override;

		// Run commands
		void executeCommandList(CommandList& list) override;

		ID3D12CommandQueue& nativeQueue() const { return *m_d3d12CommandQueue.Get(); }
	private:
		ComPtr<ID3D12CommandQueue> m_d3d12CommandQueue;
	};
}