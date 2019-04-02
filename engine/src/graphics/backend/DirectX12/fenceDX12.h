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
#include "../fence.h"

#include <d3d12.h>
#include <wrl.h>

namespace rev::gfx {

	struct FenceDX12 : public Fence
	{
		FenceDX12(Microsoft::WRL::ComPtr<ID3D12Fence> dx12Fence, HANDLE osEvent)
			: m_dx12Fence(dx12Fence)
			, m_event(osEvent)
		{}

		void waitForValue(uint64_t awaitValue, std::chrono::milliseconds timeout) override
		{
			if (m_dx12Fence->GetCompletedValue() < awaitValue)
			{
				m_dx12Fence->SetEventOnCompletion(awaitValue, m_event);
				// Spin on this event
				::WaitForSingleObject(m_event, static_cast<DWORD>(timeout.count()));
			}
		}

		Microsoft::WRL::ComPtr<ID3D12Fence> m_dx12Fence;
		HANDLE m_event;
		uint64_t m_lastValue = 0;
	};

}
