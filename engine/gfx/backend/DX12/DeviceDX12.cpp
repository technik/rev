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

        return true;
    }

    ComPtr<IDXGISwapChain4> DeviceDX12::initSwapChain(void* nativeWindowHandle,
        const math::Vec2u& resolution,
        IDXGIFactory6& dxgiFactory,
        CommandQueueDX12& commandQueue,
        const Context::SwapChainOptions& swapChainOptions)
    {
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = resolution.x();
        swapChainDesc.Height = resolution.y();
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // TODO: Support HDR
        swapChainDesc.Stereo = FALSE;
        swapChainDesc.SampleDesc = { 1, 0 };
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = kNumSwapChainBuffers;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        // It is recommended to always allow tearing if tearing support is available.
        swapChainDesc.Flags = swapChainOptions.vSync ? 0 : DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

        ComPtr<IDXGISwapChain1> swapChain1;
        HWND hWnd = (HWND)nativeWindowHandle;
        if (FAILED(dxgiFactory.CreateSwapChainForHwnd(
            &commandQueue.nativeQueue(),
            hWnd,
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapChain1)))
        {
            return nullptr;
        }

        // Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
        // will be handled manually.
        if (FAILED(dxgiFactory.MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER)))
        {
            return nullptr;
        }

        ComPtr<IDXGISwapChain4> dxgiSwapChain4;
        if (FAILED(swapChain1.As(&dxgiSwapChain4)))
        {
            return nullptr;
        }

        return dxgiSwapChain4;
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

    void DeviceDX12::UpdateRenderTargetViews(ComPtr<IDXGISwapChain4> swapChain, ComPtr<ID3D12DescriptorHeap> descriptorHeap)
    {
        auto rtvDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart());

        for (int i = 0; i < kNumSwapChainBuffers; ++i)
        {
            ComPtr<ID3D12Resource> backBuffer;
            ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

            m_d3d12Device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

            m_BackBuffers[i] = backBuffer;

            rtvHandle.Offset(rtvDescriptorSize);
        }
    }
}