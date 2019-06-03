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
#include "deviceDirectX12.h"
#include <iostream>
#include <d3d12.h>

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
#ifdef _DEBUG
		enableDebugLayer();
#endif // _DEBUG

		createDeviceFactory();
	}

	//----------------------------------------------------------------------------------------------
	int DirectX12Driver::enumeratePhysicalDevices(PhysicalDeviceInfo* devices, int maxDevices)
	{
		ComPtr<IDXGIAdapter1> dxgiAdapter1;
		int physicalDevices = 0;

		for (UINT i = 0; physicalDevices < maxDevices && m_dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
			dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

			// Check to see if the adapter can create a D3D12 device without actually 
			// creating it. The adapter with the largest dedicated video memory
			// is favored.
			if(dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				continue; // Only care about phisical devices

			if (SUCCEEDED(D3D12CreateDevice(dxgiAdapter1.Get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr)))
			{
				devices[physicalDevices].deviceApiIndex = i;
				devices[physicalDevices].shaderModel = 5;
				devices[physicalDevices].dedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
				++physicalDevices;
			}
		}

		return physicalDevices;
	}

	//----------------------------------------------------------------------------------------------
	auto DirectX12Driver::createPhysicalDevice(int deviceIndex) -> PhysicalDevice*
	{
		ComPtr<IDXGIAdapter1> dxgiAdapter1;
		auto res = m_dxgiFactory->EnumAdapters1(deviceIndex, &dxgiAdapter1);
		// Validate index
		if(res == DXGI_ERROR_NOT_FOUND)
		{
			assert(false);
			std::cout << "Invalid physical device index. This index wasn't retrieved using DirectX12Driver::enumeratePhysicalDevices\n";
			return nullptr;
		}
		// Cast to a valid index
		auto physicalDevice = new PhysicalDeviceDX12();
		dxgiAdapter1.As(&physicalDevice->m_adapter);

		return physicalDevice;
	}

	//----------------------------------------------------------------------------------------------
	auto DirectX12Driver::createDevice(
		const PhysicalDevice& adapter,
		int numQueues,
		const CommandQueue::Info* commandQueueDescs)
		-> Device*
	{
		auto& adapterDX12 = static_cast<const PhysicalDeviceDX12&>(adapter);
		ComPtr<ID3D12Device5> d3d12Device2;
		D3D12CreateDevice(adapterDX12.m_adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&d3d12Device2));

		// Check for RT support
		D3D12_FEATURE_DATA_D3D12_OPTIONS5 featureSupportData = {};
		d3d12Device2->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &featureSupportData, sizeof(featureSupportData));
		if (featureSupportData.RaytracingTier == D3D12_RAYTRACING_TIER_NOT_SUPPORTED)
			std::cout << "Raytracing not supported\n";
		else
			std::cout << "Raytracing supported!\n";


		return new DeviceDirectX12(d3d12Device2, numQueues, commandQueueDescs);
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