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
#include "commandListDX12.h"
#include "commandQueueDX12.h"
#include "deviceDirectX12.h"
#include "doubleBufferSwapChainDX12.h"
#include "fenceDX12.h"

using namespace Microsoft::WRL;

// From DXSampleHelper.h 
// Source: https://github.com/Microsoft/DirectX-Graphics-Samples
inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw std::exception();
	}
}

namespace rev :: gfx
{
	//----------------------------------------------------------------------------------------------
	DeviceDirectX12::DeviceDirectX12(
		Microsoft::WRL::ComPtr<ID3D12Device2> d3d12Device,
		int numQueues,
		const CommandQueue::Info* commandQueueDescs)
		: m_d3d12Device(d3d12Device)
	{
		createDeviceFactory();
#if defined(_DEBUG)
		enableDebugInfo();
#endif
		// Create command queues
		m_commandQueues.resize(numQueues);
		for(int i = 0; i < numQueues; ++i)
		{
			m_commandQueues[i] = createCommandQueue(commandQueueDescs[i]);
		}
	}

	//----------------------------------------------------------------------------------------------
	DoubleBufferSwapChain* DeviceDirectX12::createSwapChain(
		HWND window,
		int commandQueueIndex,
		const DoubleBufferSwapChain::Info& info)
	{
		// Swap chain desc
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = info.size.x();
		swapChainDesc.Height = info.size.y();
		assert(info.pixelFormat.channel == Image::ChannelFormat::Byte);
		assert(info.pixelFormat.numChannels == 4);
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.Stereo = info.stereo;
		assert(info.numSamples == 1); // Multisampling not supported in dx12?
		swapChainDesc.SampleDesc = {1, 0};
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

		// Create swap chain
		ComPtr<IDXGISwapChain1> swapChain1;
		m_dxgiFactory->CreateSwapChainForHwnd(
			m_commandQueues[commandQueueIndex]->m_dx12Queue.Get(),
			window,
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain1);

		m_dxgiFactory->MakeWindowAssociation(window, DXGI_MWA_NO_ALT_ENTER);

		ComPtr<IDXGISwapChain4> dxgiSwapChain4;
		swapChain1.As(&dxgiSwapChain4);
		return new DoubleBufferSwapChainDX12(*this, dxgiSwapChain4);
	}

	//----------------------------------------------------------------------------------------------
	CommandList* DeviceDirectX12::createCommandList(CommandList::Type commandType)
	{
		D3D12_COMMAND_LIST_TYPE dxType;
		switch(commandType)
		{
			case CommandList::Graphics:
				dxType = D3D12_COMMAND_LIST_TYPE_DIRECT;
				break;
			case CommandList::Compute:
				dxType = D3D12_COMMAND_LIST_TYPE_COMPUTE;
				break;
			case CommandList::Copy:
				dxType = D3D12_COMMAND_LIST_TYPE_COPY;
				break;
			case CommandList::Bundle:
				dxType = D3D12_COMMAND_LIST_TYPE_BUNDLE;
				break;
		}
		auto allocator = createCommandAllocator(dxType);
		ComPtr<ID3D12GraphicsCommandList> commandList;
		ThrowIfFailed(m_d3d12Device->CreateCommandList(0, dxType, allocator.Get(), nullptr, IID_PPV_ARGS(&commandList)));

		ThrowIfFailed(commandList->Close());

		return new CommandListDX12(allocator, commandList);
	}

	//----------------------------------------------------------------------------------------------
	auto DeviceDirectX12::createFence() -> Fence*
	{
		ComPtr<ID3D12Fence> fence;

		ThrowIfFailed(m_d3d12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

		HANDLE fenceEvent;

		fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		assert(fenceEvent && "Failed to create fence event.");

		return new FenceDX12(fence, fenceEvent);
	}

	//----------------------------------------------------------------------------------------------
	auto DeviceDirectX12::createDescriptorHeap(size_t numDescriptors, DescriptorHeap::Type type) -> DescriptorHeap*
	{
		ComPtr<ID3D12DescriptorHeap> dx12DescriptorHeap;

		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = numDescriptors;

		desc.Type = (D3D12_DESCRIPTOR_HEAP_TYPE)type;
		//desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		ThrowIfFailed(m_d3d12Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&dx12DescriptorHeap)));

		return new DescriptorHeap (dx12DescriptorHeap);
	}

	//----------------------------------------------------------------------------------------------
	void DeviceDirectX12::createRenderTargetViews(DescriptorHeap& heap, size_t n, ComPtr<ID3D12Resource>* images)
	{
		auto rtvDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(heap.m_dx12Heap->GetCPUDescriptorHandleForHeapStart());

		for(size_t i = 0; i < n; ++i)
		{
			m_d3d12Device->CreateRenderTargetView(images[i].Get(), nullptr, rtvHandle);
			rtvHandle.ptr += rtvDescriptorSize;
		}
	}

	//----------------------------------------------------------------------------------------------
	ComPtr<ID3D12CommandAllocator> DeviceDirectX12::createCommandAllocator(D3D12_COMMAND_LIST_TYPE type)
	{
		ComPtr<ID3D12CommandAllocator> commandAllocator;
		m_d3d12Device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator));
		return commandAllocator;
	}

	//----------------------------------------------------------------------------------------------
	CommandQueueDX12* DeviceDirectX12::createCommandQueue(const CommandQueue::Info& queueInfo)
	{
		D3D12_COMMAND_QUEUE_DESC dx12Desc = {};
		switch(queueInfo.type)
		{
			case CommandQueue::Graphics:
				dx12Desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
				break;
			case CommandQueue::Compute:
				dx12Desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
				break;
			case CommandQueue::Copy:
				dx12Desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
				break;
		}
		switch(queueInfo.priority) {
			case CommandQueue::Normal:
				dx12Desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
				break;
			case CommandQueue::High:
				dx12Desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
				break;
			case CommandQueue::RealTime:
				dx12Desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_GLOBAL_REALTIME;
				break;
		}
		ComPtr<ID3D12CommandQueue> dx12CommandQueue;
		m_d3d12Device->CreateCommandQueue(&dx12Desc, IID_PPV_ARGS(&dx12CommandQueue));

		return new CommandQueueDX12(dx12CommandQueue);
	}

	//----------------------------------------------------------------------------------------------
	void DeviceDirectX12::enableDebugInfo()
	{
		ComPtr<ID3D12InfoQueue> pInfoQueue;
		if (m_d3d12Device.As(&pInfoQueue) >= 0)
		{
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
			// Suppress whole categories of messages
			//D3D12_MESSAGE_CATEGORY Categories[] = {};

			// Suppress messages based on their severity level
			D3D12_MESSAGE_SEVERITY Severities[] =
			{
				D3D12_MESSAGE_SEVERITY_INFO
			};

			// Suppress individual messages by their ID
			D3D12_MESSAGE_ID DenyIds[] = {
				D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
				D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
				D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
			};

			D3D12_INFO_QUEUE_FILTER NewFilter = {};
			//NewFilter.DenyList.NumCategories = _countof(Categories);
			//NewFilter.DenyList.pCategoryList = Categories;
			NewFilter.DenyList.NumSeverities = _countof(Severities);
			NewFilter.DenyList.pSeverityList = Severities;
			NewFilter.DenyList.NumIDs = _countof(DenyIds);
			NewFilter.DenyList.pIDList = DenyIds;

			ThrowIfFailed(pInfoQueue->PushStorageFilter(&NewFilter));
		}
	}

	//----------------------------------------------------------------------------------------------
	void DeviceDirectX12::createDeviceFactory()
	{
		// Create device factory
		UINT factoryFlags = 0;
#ifdef _DEBUG
		factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif // _DEBUG
		CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&m_dxgiFactory));
	}
}