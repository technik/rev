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

#include "../Windows/windowsPlatform.h"
#include "../doubleBufferSwapChain.h"
#include "deviceDirectX12.h"
#include "GpuBufferDX12.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

namespace rev::gfx
{

	class DoubleBufferSwapChainDX12 : public DoubleBufferSwapChain
	{
	public:
		DoubleBufferSwapChainDX12(DeviceDirectX12& device, Microsoft::WRL::ComPtr<IDXGISwapChain4> dxgiSwapChain4);

		GpuBuffer* backBuffer(size_t ndx) override { return m_backBuffers[ndx]; }

		void present() override;
		int getCurrentBackBuffer() override;

	private:
		DeviceDirectX12::DescriptorHeap* m_rtvHeap;

		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_dxSwapChain;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_dxgiSwapChain4;
		GpuBufferDX12* m_backBuffers[2];

		int m_backBufferNdx = 0;
	};
}