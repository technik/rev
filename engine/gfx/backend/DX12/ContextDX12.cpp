//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2023 Carmelo J Fdez-Aguera
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
#include "ContextDX12.h"

#include "d3dx12.h"
#include <dxgi1_6.h>

#include <wrl.h>
#include <core/tools/log.h>

template<class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

namespace rev::gfx
{
    bool ContextDX12::init(bool useValidationLayers)
    {
        if (!initPhysicalDevice(useValidationLayers))
        {
            return false;
        }

        if (!initLogicalDevice(useValidationLayers))
        {
            return false;
        }

        return true;
    }

    bool ContextDX12::initPhysicalDevice(bool useValidationLayers)
    {
        // Create device factory
        ComPtr<IDXGIFactory6> dxgiFactory;

        UINT factoryFlags = 0;
        if (useValidationLayers)
        {
            factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }

        CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&dxgiFactory));

        // Iterate over available adapters
        ComPtr<IDXGIAdapter1> dxgiAdapter1;
        
        if(dxgiFactory->EnumAdapterByGpuPreference(0,
            DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
            IID_PPV_ARGS(&dxgiAdapter1)) == DXGI_ERROR_NOT_FOUND)
        {
            return false;
        }

        // TODO: Get device info
        //DXGI_ADAPTER_DESC1 adapterDesc;
        //dxgiAdapter1->GetDesc1(&adapterDesc);
        //std::wstring descMsg = adapterDesc.Description;
        //std::cout << descMsg << std::endl;

        dxgiAdapter1.As(&m_dxgiAdapter);
        return true;
    }

    bool ContextDX12::initLogicalDevice(bool breakOnValidation)
    {
        if (breakOnValidation)
        {

            ComPtr<ID3D12Debug> spDebugController0;
            ComPtr<ID3D12Debug1> spDebugController1;
            if (!SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&spDebugController0))))
            {
                return false;
            }
            if (!SUCCEEDED(spDebugController0->QueryInterface(IID_PPV_ARGS(&spDebugController1))))
            {
                return false;
            }
            spDebugController1->SetEnableGPUBasedValidation(true);
            spDebugController1->EnableDebugLayer();
        }

        ComPtr<ID3D12Device2> d3d12Device;
        auto hRes = D3D12CreateDevice(m_dxgiAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&d3d12Device));
        if (!SUCCEEDED(hRes))
        {
            return false;
        }

        if (breakOnValidation)
        {
            ComPtr<ID3D12InfoQueue> pInfoQueue;
            if (SUCCEEDED(d3d12Device->QueryInterface(IID_PPV_ARGS(&pInfoQueue))))
            {
                pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
                pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
                pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

                //D3D12_MESSAGE_CATEGORY Categories[] = {};
                D3D12_MESSAGE_SEVERITY Severities[] =
                {
                    D3D12_MESSAGE_SEVERITY_INFO
                };

                // Suppress individual messages by their ID
                //D3D12_MESSAGE_ID DenyIds[] = {};

                D3D12_INFO_QUEUE_FILTER NewFilter = {};
                //NewFilter.DenyList.NumCategories = _countof(Categories);
                //NewFilter.DenyList.pCategoryList = Categories;
                NewFilter.DenyList.NumSeverities = _countof(Severities);
                NewFilter.DenyList.pSeverityList = Severities;
                //NewFilter.DenyList.NumIDs = _countof(DenyIds);
                //NewFilter.DenyList.pIDList = DenyIds;

                if (!SUCCEEDED(pInfoQueue->PushStorageFilter(&NewFilter)))
                {
                    return false;
                }
            }
            else
            {
                std::cout << "Unable to set dx12 validation\n";

                return true;
            }
        }

        return true;
    }
}