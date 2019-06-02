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
// DirectX 12 specific headers.
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

#include "../device.h"
#include "../Windows/windowsPlatform.h"
#include "../commandQueue.h"

#include "commandQueueDX12.h"
#include "gpuTypesDX12.h"

#include <vector>

#include <wrl.h>

namespace rev :: gfx
{
	class RenderTargetViewDX12;

	class DeviceDirectX12 : public Device
	{
	public:
		DeviceDirectX12(Microsoft::WRL::ComPtr<ID3D12Device2> d3d12Device, int numQueues, const CommandQueue::Info* commandQueueDescs);

		DoubleBufferSwapChain* createSwapChain(HWND window, int commandQueueIndex, const DoubleBufferSwapChain::Info&) override;
		CommandPool* createCommandPool(CommandList::Type commandType) override;
		CommandList* createCommandList(CommandList::Type commandType, CommandPool& cmdPool) override;
		Fence* createFence() override;
		CommandQueue& commandQueue(size_t index) const override { return *m_commandQueues[index]; }

		// Resources
		DescriptorHeap* createDescriptorHeap(uint32_t numDescriptors, DescriptorType, bool shaderVisible) override;
		RenderTargetView* createRenderTargetView(DescriptorHeap& heap, uint32_t* offset, RenderTargetType rtType, const GpuBuffer& image) override;

		GpuBuffer* createCommitedResource(BufferType bufferType, size_t bufferSize) override;
		GpuBuffer* createDepthBuffer(math::Vec2u& size) override;
		GpuBuffer* createRenderTargetBuffer(math::Vec2u& size, PixelFormat format) override;

		// Pipelines
		Pipeline* createPipeline(const Pipeline::PipielineDesc&) override;

	public: // DirectX 12 specific
		static DXGI_FORMAT dxgiFromPixelFormat(PixelFormat format);
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> createCommandAllocator(D3D12_COMMAND_LIST_TYPE type);

	private:
		CommandQueueDX12* createCommandQueue(const CommandQueue::Info& queueInfo);
		Microsoft::WRL::ComPtr<ID3D12RootSignature> createRootSignature(const Pipeline::PipielineDesc& desc);
		bool compileShaderCode(const std::vector<std::string>& code, ID3DBlob*& shaderModuleBlob, const char* target);
		void enableDebugInfo();
		void createDeviceFactory();

		Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
		Microsoft::WRL::ComPtr<ID3D12Device2> m_d3d12Device;
		std::vector<CommandQueueDX12*> m_commandQueues;
	};
}
