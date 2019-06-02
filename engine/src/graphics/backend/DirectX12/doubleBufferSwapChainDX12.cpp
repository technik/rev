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
#include "doubleBufferSwapChainDX12.h"
#include "deviceDirectX12.h"

#include <wrl.h>

using namespace Microsoft::WRL;

namespace rev ::gfx
{
	//----------------------------------------------------------------------------------------------
	DoubleBufferSwapChainDX12::DoubleBufferSwapChainDX12(DeviceDirectX12& device, Microsoft::WRL::ComPtr<IDXGISwapChain4> dxgiSwapChain4)
		: m_dxgiSwapChain4(dxgiSwapChain4)
	{
		// Allocate a descriptor heap for the image views in the chain
		m_rtvHeap = device.createDescriptorHeap(2, DescriptorHeapDX12::Type::RenderTarget);

		// Retrieve swap chain buffers
		ComPtr<ID3D12Resource> backBuffers[2];
		for (int i = 0; i < 2; ++i)
		{
			m_dxgiSwapChain4->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
			m_backBuffers[i] = new GpuBufferDX12(backBuffers[i]);
		}

		// Create views
		device.createRenderTargetViews(*m_rtvHeap, 2, backBuffers, m_renderTarget);
	}

	//----------------------------------------------------------------------------------------------
	void DoubleBufferSwapChainDX12::present()
	{
		m_dxgiSwapChain4->Present(1, 0);
		m_backBufferNdx ^= 1;
	}

	//----------------------------------------------------------------------------------------------
	int DoubleBufferSwapChainDX12::getCurrentBackBuffer()
	{
		return m_backBufferNdx;
	}
}