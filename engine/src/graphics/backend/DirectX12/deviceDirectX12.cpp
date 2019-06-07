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
#include <d3dcompiler.h>
#include "d3dx12.h"

#include "commandListDX12.h"
#include "commandQueueDX12.h"
#include "deviceDirectX12.h"
#include "doubleBufferSwapChainDX12.h"
#include "fenceDX12.h"
#include "gpuTypesDX12.h"
#include "../../scene/renderGeom.h"

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
		Microsoft::WRL::ComPtr<ID3D12Device5> d3d12Device,
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
		swapChainDesc.Format = dxgiFromDataFormat(info.pixelFormat);
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
		ComPtr<ID3D12GraphicsCommandList4> commandList;
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
	GpuBuffer* DeviceDirectX12::createCommitedResource(BufferType bufferType, size_t bufferSize)
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
		CD3DX12_RESOURCE_DESC bufferResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_NONE);

		ComPtr<ID3D12Resource> dstResource;
		m_d3d12Device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &bufferResourceDesc, defaultState, nullptr, IID_PPV_ARGS(&dstResource));

		return new GpuBufferDX12(dstResource);
	}

	//----------------------------------------------------------------------------------------------
	GpuBuffer* DeviceDirectX12::createDepthBuffer(math::Vec2u& size)
	{
		// Default resource state
		D3D12_RESOURCE_STATES defaultState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

		// Resource descriptor
		CD3DX12_RESOURCE_DESC bufferResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_D32_FLOAT,
			size.x(), size.y(),
			1, 0, 1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		D3D12_CLEAR_VALUE optimizedClearValue = {};
		optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		optimizedClearValue.DepthStencil = { 0.0f, 0 };

		ComPtr<ID3D12Resource> dstResource;
		m_d3d12Device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&bufferResourceDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&optimizedClearValue,
			IID_PPV_ARGS(&dstResource));

		return new GpuBufferDX12(dstResource);
	}

	//----------------------------------------------------------------------------------------------
	GpuBuffer* DeviceDirectX12::createRenderTargetBuffer(math::Vec2u& size, DataFormat format)
	{
		// Resource descriptor
		DXGI_FORMAT pixelFormat = dxgiFromDataFormat(format);
		CD3DX12_RESOURCE_DESC bufferResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			dxgiFromDataFormat(format),
			size.x(), size.y(), 1, 0, 1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
		);

		ComPtr<ID3D12Resource> dstResource;
		m_d3d12Device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&bufferResourceDesc,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			nullptr, IID_PPV_ARGS(&dstResource));

		return new GpuBufferDX12(dstResource);
	}

	//----------------------------------------------------------------------------------------------
	RootSignature* DeviceDirectX12::createRootSignature(const RootSignature::Desc& desc)
	{
		// Allow input layout and deny unnecessary access to certain pipeline stages.
		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

		// Init root constant parameters
		CD3DX12_ROOT_PARAMETER1 rootParameters[RootSignature::MAX_CONSTANTS];

		int registerSlot = 0;
		for (uint32_t i = 0; i < desc.numConstants; ++i)
		{
			auto& parameter = desc.constants[i];
			assert(parameter.byteSize % 16 == 0);
			rootParameters[i].InitAsConstants(parameter.byteSize / 4, registerSlot, 0, D3D12_SHADER_VISIBILITY_ALL);
			int numRegisters = parameter.byteSize / 16;
			registerSlot += 1;// numRegisters;
		}

		// Init root signature descriptor
		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
		rootSignatureDescription.Init_1_1((UINT)desc.numConstants, rootParameters, 0, nullptr, rootSignatureFlags);

		// Serialize the root signature.
		ComPtr<ID3DBlob> rootSignatureBlob;
		ComPtr<ID3DBlob> errorBlob;
		ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription,
			D3D_ROOT_SIGNATURE_VERSION_1_1, &rootSignatureBlob, &errorBlob));

		// Actual creation
		ComPtr<ID3D12RootSignature> rootSignature;
		ThrowIfFailed(m_d3d12Device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
			rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));

		return new RootSignatureDX12{ rootSignature };
	}
	
	//----------------------------------------------------------------------------------------------
	// TODO: Maybe stuff all this code into a PipelineDX12 class
	auto DeviceDirectX12::createPipeline(const RasterPipeline::Desc& desc) -> RasterPipeline*
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

		// Configure input assembler layout
		D3D12_INPUT_ELEMENT_DESC attributes[RasterPipeline::Attribute::MAX_ATTRIBUTES] = {};
		for (int i = 0; i < desc.numAttributes; ++i)
		{
			auto& attribute = attributes[i];
			auto& descAttr = desc.vtxAttributes[i];
			attribute.SemanticName = descAttr.name;
			assert(!descAttr.format.sRGB);
			attribute.Format = dxgiFromDataFormat(descAttr.format);
			attribute.InputSlot = (UINT)descAttr.binding;
			attribute.AlignedByteOffset = 0; // sizeof(rev::math::Vec3f); ?
			attribute.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA; // TODO: Per instance attributes
		}

		// Pipeline State Object
		struct PipelineStateStream
		{
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
			CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
			CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
			CD3DX12_PIPELINE_STATE_STREAM_VS VS;
			CD3DX12_PIPELINE_STATE_STREAM_PS PS;
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL DS;
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
			CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER Rasterizer;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		} pipelineStateStream;

		D3D12_RT_FORMAT_ARRAY rtvFormats = {};
		rtvFormats.NumRenderTargets = 1;
		rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

		pipelineStateStream.pRootSignature = static_cast<RootSignatureDX12*>(desc.signature)->m_signature.Get();
		pipelineStateStream.InputLayout = { attributes, (UINT)desc.numAttributes };
		pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob);
		pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob);
		D3D12_DEPTH_STENCIL_DESC depthDesc;
		depthDesc.DepthEnable = TRUE;
		depthDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		depthDesc.StencilEnable = FALSE;
		pipelineStateStream.DS = CD3DX12_DEPTH_STENCIL_DESC(depthDesc);
		pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		D3D12_RASTERIZER_DESC rasterizer = {};
		rasterizer.FillMode = D3D12_FILL_MODE_SOLID;
		rasterizer.CullMode = D3D12_CULL_MODE_NONE;
		rasterizer.FrontCounterClockwise = true;
		pipelineStateStream.Rasterizer = CD3DX12_RASTERIZER_DESC(rasterizer);
		pipelineStateStream.RTVFormats = rtvFormats;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
	sizeof(PipelineStateStream), &pipelineStateStream
		};
		ComPtr<ID3D12PipelineState> pipelineState;
		ThrowIfFailed(m_d3d12Device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&pipelineState)));

		PipelineDX12* pipeline = new PipelineDX12();
		pipeline->m_pipelineState = pipelineState;

		return pipeline;
	}

    //----------------------------------------------------------------------------------------------
    RTBottomLevelAS* DeviceDirectX12::createBottomLevelAS(const RenderGeom* primitive)
    {
        D3D12_RAYTRACING_GEOMETRY_DESC descriptor = {};
        descriptor.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
        auto& geomVtx = primitive->attributes()[0];
        auto vtxData = static_cast<const GpuBufferDX12*>(geomVtx.data)->m_dx12Buffer;
        descriptor.Triangles.VertexBuffer.StartAddress =
            vtxData->GetGPUVirtualAddress() + geomVtx.offset;
        descriptor.Triangles.VertexBuffer.StrideInBytes = geomVtx.stride;
        descriptor.Triangles.VertexCount = geomVtx.count;
        descriptor.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;

        auto& indices = primitive->indices();
        auto ndxData = static_cast<const GpuBufferDX12*>(indices.data)->m_dx12Buffer;
        descriptor.Triangles.IndexBuffer = ndxData->GetGPUVirtualAddress() + indices.offset;
        assert(indices.elementSize() > 1);
        descriptor.Triangles.IndexFormat = indices.elementSize() == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
        descriptor.Triangles.IndexCount = indices.count;
        descriptor.Triangles.Transform3x4 = 0;
        descriptor.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

        // Describe the work being requested, in this case the construction of a
        // bottom-level hierarchy, with the given vertex buffers
        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS prebuildDesc;
        prebuildDesc.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
        prebuildDesc.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
        prebuildDesc.NumDescs = 1;
        prebuildDesc.pGeometryDescs = &descriptor;
        prebuildDesc.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;

        // This structure is used to hold the sizes of the required scratch memory and resulting AS
        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO info = {};

        // Building the acceleration structure (AS) requires some scratch space, as well as space to store
        // the resulting structure This function computes a conservative estimate of the memory
        // requirements for both, based on the geometry size.
        m_d3d12Device->GetRaytracingAccelerationStructurePrebuildInfo(&prebuildDesc, &info);

        // Allocate temporary memory
        // Buffer sizes need to be 256-byte-aligned
        UINT64 scratchSize = info.ScratchDataSizeInBytes;
        scratchSize += D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - scratchSize % D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
        UINT64 resultSize = info.ResultDataMaxSizeInBytes;
        resultSize += D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - resultSize % D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;

        return nullptr;
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
	RenderTargetView* DeviceDirectX12::createRenderTargetView(DescriptorHeap& heap, uint32_t* offset, RenderTargetType rtType, const GpuBuffer& image)
	{
		auto dx12Buffer = static_cast<const GpuBufferDX12&>(image).m_dx12Buffer;
		auto dx12Heap = static_cast<DescriptorHeapDX12&>(heap).m_dx12Heap;
		
		auto rtv = new RenderTargetViewDX12();
		rtv->handle = dx12Heap->GetCPUDescriptorHandleForHeapStart();
		if(offset)
			rtv->handle.ptr += *offset;
		if(rtType == RenderTargetType::Depth)
			m_d3d12Device->CreateDepthStencilView(dx12Buffer.Get(), nullptr, rtv->handle);
		else
			m_d3d12Device->CreateRenderTargetView(dx12Buffer.Get(), nullptr, rtv->handle);

		if (offset)
		{
			D3D12_DESCRIPTOR_HEAP_TYPE descriptorType = rtType == RenderTargetType::Color ? D3D12_DESCRIPTOR_HEAP_TYPE_RTV : D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			*offset += m_d3d12Device->GetDescriptorHandleIncrementSize(descriptorType);
		}

		return rtv;
	}

	//----------------------------------------------------------------------------------------------
	DXGI_FORMAT DeviceDirectX12::dxgiFromDataFormat(DataFormat format)
	{
		switch (format.components)
		{
		case 1:
		{
			switch (format.componentType)
			{
			case ScalarType::float32:
				return DXGI_FORMAT_R32_FLOAT;
			}
			break;
		}
		case 3:
		{
			switch (format.componentType)
			{
			case ScalarType::float32:
				return DXGI_FORMAT_R32G32B32_FLOAT;
			}
			break;
		}
		case 4:
		{
			switch (format.componentType)
			{
			case ScalarType::uint8:
			{
				if (format.sRGB)
					return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
				else
					return DXGI_FORMAT_R8G8B8A8_UNORM;
			}
			case ScalarType::float32:
				return DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
			break;
		}
		}
		assert(false && "Unsupported pixel format");
		return DXGI_FORMAT_R8G8B8A8_UINT;
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