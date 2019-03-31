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
#ifdef _WIN32
#include "directX12Driver.h"

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
	namespace {
		//----------------------------------------------------------------------------------------------
		void enableDebugLayer()
		{
			// Always enable the debug layer before doing anything DX12 related
			// so all possible errors generated while creating DX12 objects
			// are caught by the debug layer.
			ComPtr<ID3D12Debug> debugInterface;
			ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
			debugInterface->EnableDebugLayer();
		}
	}

	//----------------------------------------------------------------------------------------------
	DirectX12Driver::DirectX12Driver()
	{
		createDeviceFactory();
	}


	//----------------------------------------------------------------------------------------------
	void DirectX12Driver::createDeviceFactory()
	{
		// Create device factory
		UINT factoryFlags = 0;
#ifdef _DEBUG
		factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif // _DEBUG
		CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&m_dxgiFactory));
	}
}

#endif // _WIN32