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
#include "commandListDX12.h"
#include "commandQueueDX12.h"
#include "deviceDirectX12.h"
#include "doubleBufferSwapChainDX12.h"
#include "pipelineDX12.h"
#include "fenceDX12.h"
#include "d3dx12.h"
#include <d3dcompiler.h>
#include <fstream>
#include <string>

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
	//----------------------------------------------------------------------------------------------
	DeviceDirectX12::DeviceDirectX12(
		Microsoft::WRL::ComPtr<ID3D12Device2> d3d12Device,
		int numQueues,
		const CommandQueue::Info* commandQueueDescs)
		: m_d3d12Device(d3d12Device)
	{
		createDeviceFactory();
#if defined(_DEBUG)
		enableDebugInfo();
#endif
		// Create command queues
		m_commandQueues.resize(numQueues);
		for(int i = 0; i < numQueues; ++i)
		{
			m_commandQueues[i] = createCommandQueue(commandQueueDescs[i]);
		}
	}

	//----------------------------------------------------------------------------------------------
	DoubleBufferSwapChain* DeviceDirectX12::createSwapChain(
		HWND window,
		int commandQueueIndex,
		const DoubleBufferSwapChain::Info& info)
	{
		// Swap chain desc
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = info.size.x();
		swapChainDesc.Height = info.size.y();
		assert(info.pixelFormat.channel == Image::ChannelFormat::Byte);
		assert(info.pixelFormat.numChannels == 4);
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.Stereo = info.stereo;
		assert(info.numSamples == 1); // Multisampling not supported in dx12?
		swapChainDesc.SampleDesc = {1, 0};
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

		// Create swap chain
		ComPtr<IDXGISwapChain1> swapChain1;
		m_dxgiFactory->CreateSwapChainForHwnd(
			m_commandQueues[commandQueueIndex]->m_dx12Queue.Get(),
			window,
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain1);

		m_dxgiFactory->MakeWindowAssociation(window, DXGI_MWA_NO_ALT_ENTER);

		ComPtr<IDXGISwapChain4> dxgiSwapChain4;
		swapChain1.As(&dxgiSwapChain4);
		return new DoubleBufferSwapChainDX12(*this, dxgiSwapChain4);
	}

	//----------------------------------------------------------------------------------------------
	CommandPool* DeviceDirectX12::createCommandPool(CommandList::Type commandType)
	{
		D3D12_COMMAND_LIST_TYPE dxType;
		switch(commandType)
		{
			case CommandList::Graphics:
				dxType = D3D12_COMMAND_LIST_TYPE_DIRECT;
				break;
			case CommandList::Compute:
				dxType = D3D12_COMMAND_LIST_TYPE_COMPUTE;
				break;
			case CommandList::Copy:
				dxType = D3D12_COMMAND_LIST_TYPE_COPY;
				break;
			case CommandList::Bundle:
				dxType = D3D12_COMMAND_LIST_TYPE_BUNDLE;
				break;
		}
		auto allocator = createCommandAllocator(dxType);

		return new CommandPoolDX12(allocator);
	}

	//----------------------------------------------------------------------------------------------
	CommandList* DeviceDirectX12::createCommandList(CommandList::Type commandType, CommandPool& cmdPool)
	{
		D3D12_COMMAND_LIST_TYPE dxType;
		switch(commandType)
		{
			case CommandList::Graphics:
				dxType = D3D12_COMMAND_LIST_TYPE_DIRECT;
				break;
			case CommandList::Compute:
				dxType = D3D12_COMMAND_LIST_TYPE_COMPUTE;
				break;
			case CommandList::Copy:
				dxType = D3D12_COMMAND_LIST_TYPE_COPY;
				break;
			case CommandList::Bundle:
				dxType = D3D12_COMMAND_LIST_TYPE_BUNDLE;
				break;
		}

		CommandPoolDX12& dx12Pool = static_cast<CommandPoolDX12&>(cmdPool);
		ComPtr<ID3D12GraphicsCommandList> commandList;
		ThrowIfFailed(m_d3d12Device->CreateCommandList(0, dxType, dx12Pool.m_cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)));

		return new CommandListDX12(commandList);
	}

	//----------------------------------------------------------------------------------------------
	auto DeviceDirectX12::createFence() -> Fence*
	{
		ComPtr<ID3D12Fence> fence;

		ThrowIfFailed(m_d3d12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

		HANDLE fenceEvent;

		fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		assert(fenceEvent && "Failed to create fence event.");

		return new FenceDX12(fence, fenceEvent);
	}

	//----------------------------------------------------------------------------------------------
	GpuBuffer* DeviceDirectX12::createCommitedResource(BufferType bufferType, ResourceFlags flags, size_t bufferSize)
	{
		// Default resource state
		D3D12_RESOURCE_STATES defaultState;
		// Heap properties
		CD3DX12_HEAP_PROPERTIES heapProperties = {};
		switch (bufferType)
		{
		case BufferType::Resident:
			heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			defaultState = D3D12_RESOURCE_STATE_COPY_DEST;
			break;
		case BufferType::Upload:
			heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
			defaultState = D3D12_RESOURCE_STATE_GENERIC_READ;
			break;
		case BufferType::ReadBack:
			heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK);
			defaultState = D3D12_RESOURCE_STATE_COPY_SOURCE;
			break;
		default:
			assert(false);
			return nullptr;
		}
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;

		// Resource descriptor
		D3D12_RESOURCE_FLAGS dx12flags = D3D12_RESOURCE_FLAG_NONE;

		// TODO: Should these flags apply to texture resources only?
		if (flags & ResourceFlags::IsRenderTarget)
			dx12flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		if (flags & ResourceFlags::IsDepthStencil)
			dx12flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		CD3DX12_RESOURCE_DESC bufferResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, dx12flags);

		ComPtr<ID3D12Resource> dstResource;
		m_d3d12Device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &bufferResourceDesc, defaultState, nullptr, IID_PPV_ARGS(&dstResource));

		return new GpuBufferDX12(dstResource);
	}
	
	//----------------------------------------------------------------------------------------------
	// TODO: Maybe stuff all this code into a PipelineDX12 class
	auto DeviceDirectX12::createPipeline(const Pipeline::PipielineDesc& desc) -> Pipeline*
	{
		// Build the shader modules first, because that can fail
		// TODO: Create shaders from code
		ID3DBlob* vertexShaderBlob;
		if(!compileShaderCode(desc.vtxCode, vertexShaderBlob, "vs_5_1"))
		{
			return nullptr;
		}
		ID3DBlob* pixelShaderBlob;
		if (!compileShaderCode(desc.pxlCode, pixelShaderBlob, "ps_5_1"))
		{
			vertexShaderBlob->Release();
			return nullptr;
		}

		// Configure input layout
		D3D12_INPUT_ELEMENT_DESC vtxPosLayout = {};
		vtxPosLayout.SemanticName = "position";
		vtxPosLayout.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vtxPosLayout.InputSlot = 0;
		vtxPosLayout.AlignedByteOffset = 0;// sizeof(rev::math::Vec3f);
		vtxPosLayout.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;

		// Create a root signature.
		ComPtr<ID3D12RootSignature> rootSignature = createRootSignature(desc);

		// Pipeline State Object
		struct PipelineStateStream
		{
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
			CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
			CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
			CD3DX12_PIPELINE_STATE_STREAM_VS VS;
			CD3DX12_PIPELINE_STATE_STREAM_PS PS;
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		} pipelineStateStream;

		D3D12_RT_FORMAT_ARRAY rtvFormats = {};
		rtvFormats.NumRenderTargets = 1;
		rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

		pipelineStateStream.pRootSignature = rootSignature.Get();
		pipelineStateStream.InputLayout = { &vtxPosLayout, 1 };
		pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob);
		pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob);
		pipelineStateStream.DSVFormat = DXGI_FORMAT_UNKNOWN;
		pipelineStateStream.RTVFormats = rtvFormats;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
	sizeof(PipelineStateStream), &pipelineStateStream
		};
		ComPtr<ID3D12PipelineState> pipelineState;
		ThrowIfFailed(m_d3d12Device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&pipelineState)));

		PipelineDX12* pipeline = new PipelineDX12();
		pipeline->m_pipelineState = pipelineState;
		pipeline->m_rootSignature = rootSignature;

		return pipeline;
	}

	//----------------------------------------------------------------------------------------------
	DescriptorHeap* DeviceDirectX12::createDescriptorHeap(uint32_t numDescriptors, DescriptorType type, bool shaderVisible)
	{
		ComPtr<ID3D12DescriptorHeap> dx12DescriptorHeap;

		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.NumDescriptors = (UINT)numDescriptors;
		switch (type)
		{
		case DescriptorType::RenderTarget:
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			break;
		case DescriptorType::DepthStencil:
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			break;
		case DescriptorType::Sampler:
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
			break;
		case DescriptorType::ShaderResource:
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			break;
		}
		if(shaderVisible && type == DescriptorType::ShaderResource)
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		ThrowIfFailed(m_d3d12Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&dx12DescriptorHeap)));

		return new DescriptorHeapDX12 (dx12DescriptorHeap);
	}

	//----------------------------------------------------------------------------------------------
	void DeviceDirectX12::createRenderTargetViews(DescriptorHeapDX12& heap, size_t n, ComPtr<ID3D12Resource>* images, RenderTargetViewDX12* rtvOut)
	{
		auto rtvDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(heap.m_dx12Heap->GetCPUDescriptorHandleForHeapStart());

		for(size_t i = 0; i < n; ++i)
		{
			m_d3d12Device->CreateRenderTargetView(images[i].Get(), nullptr, rtvHandle);
			rtvOut[i].handle = rtvHandle;
			rtvHandle.ptr += rtvDescriptorSize;
		}
	}

	//----------------------------------------------------------------------------------------------
	ComPtr<ID3D12CommandAllocator> DeviceDirectX12::createCommandAllocator(D3D12_COMMAND_LIST_TYPE type)
	{
		ComPtr<ID3D12CommandAllocator> commandAllocator;
		m_d3d12Device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator));
		return commandAllocator;
	}

	//----------------------------------------------------------------------------------------------
	CommandQueueDX12* DeviceDirectX12::createCommandQueue(const CommandQueue::Info& queueInfo)
	{
		D3D12_COMMAND_QUEUE_DESC dx12Desc = {};
		switch(queueInfo.type)
		{
			case CommandQueue::Graphics:
				dx12Desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
				break;
			case CommandQueue::Compute:
				dx12Desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
				break;
			case CommandQueue::Copy:
				dx12Desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
				break;
		}
		switch(queueInfo.priority) {
			case CommandQueue::Normal:
				dx12Desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
				break;
			case CommandQueue::High:
				dx12Desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
				break;
			case CommandQueue::RealTime:
				dx12Desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_GLOBAL_REALTIME;
				break;
		}
		ComPtr<ID3D12CommandQueue> dx12CommandQueue;
		m_d3d12Device->CreateCommandQueue(&dx12Desc, IID_PPV_ARGS(&dx12CommandQueue));

		return new CommandQueueDX12(dx12CommandQueue);
	}

	//----------------------------------------------------------------------------------------------
	ComPtr<ID3D12RootSignature> DeviceDirectX12::createRootSignature(const Pipeline::PipielineDesc& desc)
	{
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
		if (FAILED(m_d3d12Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		// Allow input layout and deny unnecessary access to certain pipeline stages.
		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

		std::vector<CD3DX12_ROOT_PARAMETER1> rootParameters;
		rootParameters.resize(desc.vtxUniforms.numUniforms + desc.pxlUniforms.numUniforms);
		int paramNdx = 0;
		// Shader space 0 is for vertex shader, and shader space 1 is for pixel shaders
		int shaderRegister = 0;
		for (uint32_t i = 0; i < desc.vtxUniforms.numUniforms; ++i)
		{
			auto& parameter = desc.vtxUniforms.uniform[i];
			assert(parameter.byteSize() % 16 == 0);
			int numRegisters = parameter.byteSize() / 16;
			rootParameters[paramNdx++].InitAsConstants(parameter.byteSize()/4, shaderRegister, 0, D3D12_SHADER_VISIBILITY_VERTEX);
			shaderRegister += numRegisters;
		}
		shaderRegister = 0;
		for (uint32_t i = 0; i < desc.pxlUniforms.numUniforms; ++i)
		{
			auto& parameter = desc.pxlUniforms.uniform[i];
			assert(parameter.byteSize() % 16 == 0);
			int numRegisters = parameter.byteSize() / 16;
			rootParameters[paramNdx++].InitAsConstants(parameter.byteSize()/4, shaderRegister, 1, D3D12_SHADER_VISIBILITY_PIXEL);
			shaderRegister += numRegisters;
		}

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
		//rootSignatureDescription.Init_1_1(0, nullptr, 0, nullptr, rootSignatureFlags);
		rootSignatureDescription.Init_1_1((UINT)rootParameters.size(), rootParameters.data(), 0, nullptr, rootSignatureFlags);

		// Serialize the root signature.
		ComPtr<ID3DBlob> rootSignatureBlob;
		ComPtr<ID3DBlob> errorBlob;
		ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription,
			featureData.HighestVersion, &rootSignatureBlob, &errorBlob));
		// Create the root signature.
		ComPtr<ID3D12RootSignature> rootSignature;
		ThrowIfFailed(m_d3d12Device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
			rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));

		return rootSignature;
	}

	//----------------------------------------------------------------------------------------------
	bool DeviceDirectX12::compileShaderCode(const std::vector<std::string>& code, ID3DBlob*& shaderModuleBlob, const char* target)
	{
		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		flags |= D3DCOMPILE_DEBUG;
#endif
		// Bypass DX restriction of only being able to compile shaders from file:
		// Save code into a file, then load it from there
		std::ofstream shaderFile("temp-shader.hlsl");
		for (auto& segment : code)
		{
			shaderFile << segment << "\n";
		}
		shaderFile.close();

		shaderModuleBlob = nullptr;
		ID3DBlob* errorBlob = nullptr;
		HRESULT hr = D3DCompileFromFile(L"temp-shader.hlsl", nullptr, nullptr, "main", target, flags, 0, &shaderModuleBlob, &errorBlob);
		// Prefer higher CS shader profile when possible as CS 5.0 provides better performance on 11-class hardware.
		if (FAILED(hr))
		{
			if (errorBlob)
			{
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
				errorBlob->Release();
			}

			if (shaderModuleBlob)
				shaderModuleBlob->Release();

			return false;
		}

		//const D3D_SHADER_MACRO defines[] = {};
		return true;
	}

	//----------------------------------------------------------------------------------------------
	void DeviceDirectX12::enableDebugInfo()
	{
		ComPtr<ID3D12InfoQueue> pInfoQueue;
		if (m_d3d12Device.As(&pInfoQueue) >= 0)
		{
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
			// Suppress whole categories of messages
			//D3D12_MESSAGE_CATEGORY Categories[] = {};

			// Suppress messages based on their severity level
			D3D12_MESSAGE_SEVERITY Severities[] =
			{
				D3D12_MESSAGE_SEVERITY_INFO
			};

			// Suppress individual messages by their ID
			D3D12_MESSAGE_ID DenyIds[] = {
				D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
				D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
				D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
			};

			D3D12_INFO_QUEUE_FILTER NewFilter = {};
			//NewFilter.DenyList.NumCategories = _countof(Categories);
			//NewFilter.DenyList.pCategoryList = Categories;
			NewFilter.DenyList.NumSeverities = _countof(Severities);
			NewFilter.DenyList.pSeverityList = Severities;
			NewFilter.DenyList.NumIDs = _countof(DenyIds);
			NewFilter.DenyList.pIDList = DenyIds;

			ThrowIfFailed(pInfoQueue->PushStorageFilter(&NewFilter));
		}
	}

	//----------------------------------------------------------------------------------------------
	void DeviceDirectX12::createDeviceFactory()
	{
		// Create device factory
		UINT factoryFlags = 0;
#ifdef _DEBUG
		factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif // _DEBUG
		CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&m_dxgiFactory));
	}
}