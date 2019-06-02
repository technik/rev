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

#include "../gpuTypes.h"
#include <d3d12.h>
#include <wrl.h>

namespace rev::gfx
{
	//-------------------------------------------------------------------------------------------------
	class CommandPoolDX12 : public CommandPool
	{
	public:
		CommandPoolDX12(Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_cmdAllocator)
			: m_cmdAllocator(m_cmdAllocator)
		{}

		void reset() override
		{
			m_cmdAllocator->Reset();
		}

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_cmdAllocator;
	private:
	};

	//-------------------------------------------------------------------------------------------------
	struct DescriptorHeapDX12 : DescriptorHeap
	{
		enum class Type : int
		{
			ShaderResource = (int)D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			Sampler = (int)D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
			RenderTarget = (int)D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			DepthStencil = (int)D3D12_DESCRIPTOR_HEAP_TYPE_DSV
		};

		DescriptorHeapDX12(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& dx12)
			: m_dx12Heap(dx12)
		{}

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dx12Heap;
	};

	//-------------------------------------------------------------------------------------------------
	struct GpuBufferDX12 : public GpuBuffer
	{
		GpuBufferDX12(Microsoft::WRL::ComPtr<ID3D12Resource> dx12Buffer)
			: m_dx12Buffer(dx12Buffer)
		{}

		Microsoft::WRL::ComPtr<ID3D12Resource> m_dx12Buffer;
	};

	//-------------------------------------------------------------------------------------------------
	class RenderTargetViewDX12 : public RenderTargetView
	{
	public:
		D3D12_CPU_DESCRIPTOR_HANDLE handle;
	};
}

