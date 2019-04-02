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
#include "../commandList.h"

#include <d3d12.h>
#include "../Windows/windowsPlatform.h"
#include <wrl.h>

namespace rev::gfx {

	class CommandListDX12 : public CommandList
	{
	public:
		CommandListDX12(Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_cmdAllocator, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> dx12CmdList)
			: m_cmdAllocator(m_cmdAllocator)
			, m_commandList(dx12CmdList)
		{}

		void reset() override
		{
			m_cmdAllocator->Reset();
			m_commandList->Reset(m_cmdAllocator.Get(), nullptr);
		}

		// --- Commands ---
		void resourceBarrier(GpuBuffer* resource, Barrier barrierType, ResourceState before, ResourceState after) override;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
	private:
		uint64_t m_lastValue = 0;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_cmdAllocator;
	};
}

