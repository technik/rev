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

#include <core/tools/log.h>
#include <core/string_util.h>

#include "CommandQueueDX12.h"
#include "DeviceDX12.h"

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
        m_device = m_device12;

        // Create command queues
        m_GfxQueue = static_cast<CommandQueueDX12*>(m_device12->createCommandQueue({ CommandQueue::Type::Graphics, CommandQueue::Priority::High }));
        m_AsyncComputeQueue = static_cast<CommandQueueDX12*>(m_device12->createCommandQueue({ CommandQueue::Type::Compute, CommandQueue::Priority::High }));
        m_CopyQueue = static_cast<CommandQueueDX12*>(m_device12->createCommandQueue({ CommandQueue::Type::Copy, CommandQueue::Priority::Normal }));

        return true;
    }

    void ContextDX12::end()
    {
        // TODO
        // Assert no pending swapchains (or end them?)
        // Assert no pending work on queues (or wait?)
        // Destroy command queues
        delete m_GfxQueue;
        delete m_AsyncComputeQueue;
        delete m_CopyQueue;
        
        // Shutdown device
        m_device12->end();
        delete m_device12;
        m_device12 = nullptr;
        m_device = nullptr; // Clean base class accessor
    }

    bool ContextDX12::initPhysicalDevice(bool useValidationLayers)
    {
        // Create device factory
        UINT factoryFlags = 0;
        if (useValidationLayers)
        {
            factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }

        CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&m_dxgiFactory));

        // Iterate over available adapters
        ComPtr<IDXGIAdapter1> dxgiAdapter;
        
        if(m_dxgiFactory->EnumAdapterByGpuPreference(0,
            DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
            IID_PPV_ARGS(&dxgiAdapter)) == DXGI_ERROR_NOT_FOUND)
        {
            std::cout << "Unable to find graphics adapter\n";
            return false;
        }

        if (!SUCCEEDED(dxgiAdapter.As(&m_dxgiAdapter)))
        {
            std::cout << "Unsupported API level\n";
            return false;
        }

        // Get device info
        DXGI_ADAPTER_DESC2 adapterDesc;
        m_dxgiAdapter->GetDesc2(&adapterDesc);

        // Report device information
        m_deviceInfo.name = core::fromWString(adapterDesc.Description);
        m_deviceInfo.dediactedVideoMemory = adapterDesc.DedicatedVideoMemory;
        
        // Check for vSync off support
        bool allowTearing = false;
        if (FAILED(m_dxgiFactory->CheckFeatureSupport(
            DXGI_FEATURE_PRESENT_ALLOW_TEARING,
            &allowTearing, sizeof(allowTearing))))
        {
            allowTearing = FALSE;
        }
        m_deviceInfo.vSyncOffSupport = allowTearing;

        return true;
    }

    bool ContextDX12::initLogicalDevice(bool breakOnValidation)
    {
        m_device12 = new DeviceDX12();
        if (!m_device12->init(m_dxgiAdapter.Get(), breakOnValidation))
        {
            delete m_device12;
            return false;
        }

        return true;
    }

    //------------------------------------------------------------------------------------
    bool ContextDX12::createSwapChain(const SwapChainOptions& desc, const math::Vec2u& imageSize)
    {
        m_swapChain = std::make_unique<SwapChain12>(
            m_device12,
            nativeWindow(),
            *m_dxgiFactory.Get(),
            *m_GfxQueue,
            desc,
            imageSize);
        return true;
    }

    //------------------------------------------------------------------------------------
    ContextDX12::SwapChain12::SwapChain12(DeviceDX12* device,
        void* nativeWindowHandle,
        IDXGIFactory6& dxgiFactory,
        CommandQueueDX12& commandQueue,
        const SwapChainOptions& desc, const math::Vec2u& resolution)
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
        swapChainDesc.Flags = desc.vSync ? 0 : DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

        ComPtr<IDXGISwapChain1> swapChain1;
        HWND hWnd = (HWND)nativeWindowHandle;
        ThrowIfFailed(dxgiFactory.CreateSwapChainForHwnd(
            &commandQueue.nativeQueue(),
            hWnd,
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapChain1));

        // Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
        // will be handled manually.
        ThrowIfFailed(dxgiFactory.MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

        ThrowIfFailed(swapChain1.As(&m_nativeSwapChain));

        m_rtvDescriptorSize = device->nativeDevice().GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        UpdateResourceViews();
    }

    //------------------------------------------------------------------------------------
    void ContextDX12::SwapChain12::resize(const math::Vec2u& imageSize)
    {
        assert(false && "Not implemented");
    }

    //------------------------------------------------------------------------------------
    uint64_t ContextDX12::SwapChain12::present(CommandQueue& gfxQueue)
    {
        assert(false && "Not implemented");
        return -1;
    }

    //------------------------------------------------------------------------------------
    void ContextDX12::SwapChain12::UpdateResourceViews()
    {
        auto& device12 = static_cast<DeviceDX12&>(RenderContext().device());

        for (int i = 0; i < kNumSwapChainBuffers; ++i)
        {
            auto& buffer = m_buffers[i];
            // Release old resources
            if (buffer.resource.isValid())
            {
                device12.destroy(buffer.srv);
                device12.destroy(buffer.rtv);

                device12.releaseResource(buffer.resource);
            }

            // Acquire new resources
            ComPtr<ID3D12Resource> backBuffer;
            ThrowIfFailed(m_nativeSwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));
            buffer.resource = device12.registerResource(backBuffer);

            buffer.srv = device12.createSRV(buffer.resource);
            buffer.rtv = device12.createRTV(buffer.resource);
        }
    }
}