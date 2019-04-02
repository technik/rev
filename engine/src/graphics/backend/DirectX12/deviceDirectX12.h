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
// DirectX 12 specific headers.
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

#include "../device.h"
#include "../Windows/windowsPlatform.h"
#include "../commandQueue.h"
#include "commandQueueDX12.h"

#include <vector>

#include <wrl.h>

namespace rev :: gfx
{
	class CommandList;

	class DeviceDirectX12 : public Device
	{
	public:
		struct DescriptorHeap
		{
			enum class Type : int 
			{
				ShaderResource = (int)D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
				Sampler = (int)D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
				RenderTarget = (int)D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
				DepthStencil = (int)D3D12_DESCRIPTOR_HEAP_TYPE_DSV
			};

			DescriptorHeap(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& dx12)
				: m_dx12Heap(dx12)
			{}

			Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dx12Heap;
		};

	public:
		DeviceDirectX12(Microsoft::WRL::ComPtr<ID3D12Device2> d3d12Device, int numQueues, const CommandQueue::Info* commandQueueDescs);

		DoubleBufferSwapChain* createSwapChain(HWND window, int commandQueueIndex, const DoubleBufferSwapChain::Info&) override;
		CommandList* createCommandList(CommandList::Type commandType) override;
		Fence* createFence() override;
		CommandQueue& commandQueue(size_t index) const override { return *m_commandQueues[index]; }

	public: // DirectX 12 specific
		DescriptorHeap* createDescriptorHeap(size_t numDescriptors, DescriptorHeap::Type);

		void createRenderTargetViews(
			DescriptorHeap& heap,
			size_t n,
			Microsoft::WRL::ComPtr<ID3D12Resource>* images);

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> createCommandAllocator(D3D12_COMMAND_LIST_TYPE type);

	private:
		CommandQueueDX12* createCommandQueue(const CommandQueue::Info& queueInfo);
		void enableDebugInfo();
		void createDeviceFactory();

		Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
		Microsoft::WRL::ComPtr<ID3D12Device2> m_d3d12Device;
		std::vector<CommandQueueDX12*> m_commandQueues;
	};
}
