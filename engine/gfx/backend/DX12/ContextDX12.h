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
#pragma once

#include "../Context.h"
#include "../Device.h"
#include "CommandQueueDX12.h"

#include <dxgi1_6.h>
#include <wrl.h>

template<class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

namespace rev::gfx
{
    class DeviceDX12;

    class ContextDX12 : public Context
    {
    public:
        bool init(bool useValidationLayers);
        void end() override;

        CommandQueue& GfxQueue() const override { return *m_GfxQueue; }
        CommandQueue& AsyncComputeQueue() const override { return *m_AsyncComputeQueue; }
        CommandQueue& CopyQueue() const override { return *m_CopyQueue; }

        bool createSwapChain(const SwapChainOptions&, const math::Vec2u& imageSize) override;
        math::Vec2u resizeSwapChain(const math::Vec2u& imageSize) override { return imageSize; } // returns the actual size the swapchain was resized to
        void destroySwapChain() override {}

    private:

        class SwapChain12 : public SwapChain
        {
        public:
            SwapChain12(DeviceDX12* device,
                void* window,
                IDXGIFactory6& dxgiFactory,
                CommandQueueDX12& commandQueue, 
                const SwapChainOptions&, const math::Vec2u& imageSize);

            void resize(const math::Vec2u& imageSize) override;

            // Returns the fence value signaled after present
            void present() override;

        private:
            void UpdateResourceViews();
            
            ComPtr<IDXGISwapChain4> m_nativeSwapChain;
            unsigned m_rtvDescriptorSize;
        };

        std::unique_ptr<SwapChain> m_swapChain;

        bool initPhysicalDevice(bool useValidationLayers);
        bool initLogicalDevice(bool breakOnValidation);

        ComPtr<IDXGIAdapter4> m_dxgiAdapter;
        ComPtr<IDXGIFactory6> m_dxgiFactory;

        CommandQueueDX12* m_GfxQueue{};
        CommandQueueDX12* m_AsyncComputeQueue{};
        CommandQueueDX12* m_CopyQueue{};

        DeviceDX12* m_device12{};
    };
}