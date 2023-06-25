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
#include "DeviceDX12.h"

#include <core/tools/log.h>
#include <core/string_util.h>

#include "CommandQueueDX12.h"
#include "dx12Util.h"

namespace rev::gfx
{
    bool DeviceDX12::init(IDXGIAdapter4* physicalDevice, bool breakOnValidation)
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

        auto hRes = D3D12CreateDevice(physicalDevice, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_d3d12Device));
        if (!SUCCEEDED(hRes))
        {
            return false;
        }

        if (breakOnValidation)
        {
            ComPtr<ID3D12InfoQueue> pInfoQueue;
            if (SUCCEEDED(m_d3d12Device->QueryInterface(IID_PPV_ARGS(&pInfoQueue))))
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

        m_globalDescHeap = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_maxDescriptors);
        m_globalRTVHeap = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, m_maxRTVs);

        return true;
    }

    CommandQueue* DeviceDX12::createCommandQueue(CommandQueue::Info desc)
    {
        return new CommandQueueDX12(*m_d3d12Device.Get(), desc);
    }

    ComPtr<ID3D12DescriptorHeap> DeviceDX12::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors)
    {
        ComPtr<ID3D12DescriptorHeap> descriptorHeap;

        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.NumDescriptors = numDescriptors;
        desc.Type = type;

        ThrowIfFailed(m_d3d12Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap)));

        return descriptorHeap;
    }

    GPUResource DeviceDX12::registerResource(const ComPtr<ID3D12Resource>& dx12res)
    {
        // Reuse slots if possible
        for (int i = 0; i < m_nativeResources.size(); ++i)
        {
            if (!m_nativeResources[i]) // found empty slot
            {
                m_nativeResources[i] = dx12res;
                return GPUResource(i);
            }
        }

        // No valid slots found. Grow the queue
        m_nativeResources.push_back(dx12res);
        return GPUResource((uint32_t(m_nativeResources.size() - 1)));
    }

    void DeviceDX12::releaseResource(GPUResource handle)
    {
        assert(handle.isValid());
        m_nativeResources[handle.id] = nullptr;
    }

    // Descriptor management
    SRV DeviceDX12::createSRV(GPUResource resource)
    {
        auto baseDesc = m_globalDescHeap->GetCPUDescriptorHandleForHeapStart();
        auto desc = CD3DX12_CPU_DESCRIPTOR_HANDLE(baseDesc, m_numDescs, m_descriptorSize);

        assert(m_numDescs < m_maxDescriptors);
        m_d3d12Device->CreateShaderResourceView(getNative(resource), nullptr, desc);

        return SRV(m_numDescs++);
    }

    UAV DeviceDX12::createUAV(GPUResource resource)
    {
        auto baseDesc = m_globalDescHeap->GetCPUDescriptorHandleForHeapStart();
        auto desc = CD3DX12_CPU_DESCRIPTOR_HANDLE(baseDesc, m_numDescs, m_descriptorSize);

        assert(m_numDescs < m_maxDescriptors);
        m_d3d12Device->CreateUnorderedAccessView(getNative(resource), nullptr, nullptr, desc);

        return UAV(m_numDescs++);
    }

    RTV DeviceDX12::createRTV(GPUResource resource)
    {
        auto baseDesc = m_globalRTVHeap->GetCPUDescriptorHandleForHeapStart();
        auto desc = CD3DX12_CPU_DESCRIPTOR_HANDLE(baseDesc, m_numRTVs, m_rtvDescriptorSize);

        assert(m_numRTVs < m_maxRTVs);
        m_d3d12Device->CreateRenderTargetView(getNative(resource), nullptr, desc);

        return RTV(m_numRTVs++);
    }

    void DeviceDX12::destroy(SRV)
    {
        assert(m_numDescs);
        m_numDescs--;
    }

    void DeviceDX12::destroy(UAV)
    {
        assert(m_numDescs);
        m_numDescs--;
    }

    void DeviceDX12::destroy(RTV rtv)
    {
        assert(m_numRTVs);
        m_numRTVs--;
    }
}