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

#include "../Device.h"
#include "../Context.h"

#include "dx12Util.h"

namespace rev::gfx
{
    class CommandQueueDX12;

    class DeviceDX12 : public Device
    {
    public:
        bool init(IDXGIAdapter4* physicalDevice, bool breakOnValidation);
        void end() {}

        CommandQueue* createCommandQueue(CommandQueue::Info) override;

        // Resource management
        ID3D12Resource* getNative(GPUResource handle) { return m_nativeResources[handle.id].Get(); }
        GPUResource registerResource(const ComPtr<ID3D12Resource>&);
        void releaseResource(GPUResource);

        // Descriptor management
        SRV createSRV(GPUResource resource);
        UAV createUAV(GPUResource resource);
        RTV createRTV(GPUResource resource);

        void destroy(SRV);
        void destroy(UAV);
        void destroy(RTV);

        auto& nativeDevice() const { return *m_d3d12Device.Get(); }

    private:
        ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors);

        ComPtr<ID3D12Device2> m_d3d12Device;

        // TODO: Add generation counter?
        std::vector<ComPtr<ID3D12Resource>> m_nativeResources;
        ComPtr<ID3D12DescriptorHeap> m_globalDescHeap;
        ComPtr<ID3D12DescriptorHeap> m_globalRTVHeap;
        static constexpr size_t m_maxRTVs = 2; // Back-buffers only for now
        static constexpr size_t m_maxDescriptors = 4; // Back-buffers only for now
        int m_numRTVs = 0;
        int m_numDescs = 0;

        unsigned m_rtvDescriptorSize;
        unsigned m_descriptorSize;
    };
}