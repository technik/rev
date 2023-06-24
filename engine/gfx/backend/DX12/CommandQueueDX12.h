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

#include <queue>
#include <memory>

namespace rev::gfx
{
	class CommandListDX12 : public CommandList
	{
	public:
		CommandListDX12(ComPtr<ID3D12GraphicsCommandList2> nativeCmdList, ComPtr<ID3D12CommandAllocator> allocator)
			: m_commandList(nativeCmdList)
			, m_allocator(allocator)
		{}

		ComPtr<ID3D12GraphicsCommandList2> m_commandList;
		ComPtr<ID3D12CommandAllocator> m_allocator;
		uint64_t m_submissionFenceId = 0; // 0 means not submitted
	};

	class CommandQueueDX12 : public CommandQueue
	{
	public:
		CommandQueueDX12(ID3D12Device2& device, Info type);
		~CommandQueueDX12();

		// ---- Synchronization ----
		bool isFenceComplete(uint64_t fenceValue) override;
		void refreshInFlightWork() override;

		// Run commands
		CommandList& getCommandList() override;
		uint64_t submitCommandList(CommandList& list) override;

		ID3D12CommandQueue& nativeQueue() const { return *m_d3d12CommandQueue.Get(); }

	private:
		std::vector<std::unique_ptr<CommandListDX12>> m_freeCmdLists;
		std::vector<std::unique_ptr<CommandListDX12>> m_inFlightCmdLists;

		std::unique_ptr<CommandListDX12> createCommandList();

		ID3D12Device2& m_device;
		ComPtr<ID3D12CommandQueue> m_d3d12CommandQueue;

		D3D12_COMMAND_LIST_TYPE                     m_CommandListType;
		Microsoft::WRL::ComPtr<ID3D12Fence>         m_d3d12Fence;
		uint64_t                                    m_nextFenceValue;
		uint64_t                                    m_completedFenceValue;
	};
}