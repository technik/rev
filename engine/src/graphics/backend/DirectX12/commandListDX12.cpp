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
#include "GpuBufferDX12.h"
#include "renderTargetViewDX12.h"

namespace rev::gfx {

	namespace {
		D3D12_RESOURCE_STATES dx12StateFromCommon(CommandList::ResourceState commonState)
		{
			switch(commonState) {
				case CommandList::ResourceState::RenderTarget:
					return D3D12_RESOURCE_STATE_RENDER_TARGET;
				case CommandList::ResourceState::PixelShaderResource:
					return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
				case CommandList::ResourceState::CopySource:
					return D3D12_RESOURCE_STATE_COPY_SOURCE;
				case CommandList::ResourceState::CopyDst:
					return D3D12_RESOURCE_STATE_COPY_DEST;
				case CommandList::ResourceState::GenericRead:
					return D3D12_RESOURCE_STATE_GENERIC_READ;
				case CommandList::ResourceState::Present:
					return D3D12_RESOURCE_STATE_PRESENT;
			}

			return D3D12_RESOURCE_STATE_COMMON;
		}

		//------------------------------------------------------------------------------------------------
		// Row-by-row memcpy
		inline void MemcpySubresource(
			_In_ const D3D12_MEMCPY_DEST* pDest,
			_In_ const D3D12_SUBRESOURCE_DATA* pSrc,
			SIZE_T RowSizeInBytes,
			UINT NumRows,
			UINT NumSlices)
		{
			for (UINT z = 0; z < NumSlices; ++z)
			{
				BYTE* pDestSlice = reinterpret_cast<BYTE*>(pDest->pData) + pDest->SlicePitch * z;
				const BYTE* pSrcSlice = reinterpret_cast<const BYTE*>(pSrc->pData) + pSrc->SlicePitch * z;
				for (UINT y = 0; y < NumRows; ++y)
				{
					memcpy(pDestSlice + pDest->RowPitch * y,
						pSrcSlice + pSrc->RowPitch * y,
						RowSizeInBytes);
				}
			}
		}

		//------------------------------------------------------------------------------------------------
		struct CD3DX12_TEXTURE_COPY_LOCATION : public D3D12_TEXTURE_COPY_LOCATION
		{
			CD3DX12_TEXTURE_COPY_LOCATION() = default;
			explicit CD3DX12_TEXTURE_COPY_LOCATION(const D3D12_TEXTURE_COPY_LOCATION& o) :
				D3D12_TEXTURE_COPY_LOCATION(o)
			{}
			CD3DX12_TEXTURE_COPY_LOCATION(_In_ ID3D12Resource* pRes)
			{
				pResource = pRes;
				Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
				PlacedFootprint = {};
			}
			CD3DX12_TEXTURE_COPY_LOCATION(_In_ ID3D12Resource* pRes, D3D12_PLACED_SUBRESOURCE_FOOTPRINT const& Footprint)
			{
				pResource = pRes;
				Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
				PlacedFootprint = Footprint;
			}
			CD3DX12_TEXTURE_COPY_LOCATION(_In_ ID3D12Resource* pRes, UINT Sub)
			{
				pResource = pRes;
				Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
				SubresourceIndex = Sub;
			}
		};

		//------------------------------------------------------------------------------------------------
		// All arrays must be populated (e.g. by calling GetCopyableFootprints)
		inline UINT64 UpdateSubresources(
			_In_ ID3D12GraphicsCommandList* pCmdList,
			_In_ ID3D12Resource* pDestinationResource,
			_In_ ID3D12Resource* pIntermediate,
			_In_range_(0, D3D12_REQ_SUBRESOURCES) UINT FirstSubresource,
			_In_range_(0, D3D12_REQ_SUBRESOURCES - FirstSubresource) UINT NumSubresources,
			UINT64 RequiredSize,
			_In_reads_(NumSubresources) const D3D12_PLACED_SUBRESOURCE_FOOTPRINT* pLayouts,
			_In_reads_(NumSubresources) const UINT* pNumRows,
			_In_reads_(NumSubresources) const UINT64* pRowSizesInBytes,
			_In_reads_(NumSubresources) const D3D12_SUBRESOURCE_DATA* pSrcData)
		{
			// Minor validation
			auto IntermediateDesc = pIntermediate->GetDesc();
			auto DestinationDesc = pDestinationResource->GetDesc();
			if (IntermediateDesc.Dimension != D3D12_RESOURCE_DIMENSION_BUFFER ||
				IntermediateDesc.Width < RequiredSize + pLayouts[0].Offset ||
				RequiredSize > SIZE_T(-1) ||
				(DestinationDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER &&
				(FirstSubresource != 0 || NumSubresources != 1)))
			{
				return 0;
			}

			BYTE* pData;
			HRESULT hr = pIntermediate->Map(0, nullptr, reinterpret_cast<void**>(&pData));
			if (FAILED(hr))
			{
				return 0;
			}

			for (UINT i = 0; i < NumSubresources; ++i)
			{
				if (pRowSizesInBytes[i] > SIZE_T(-1)) return 0;
				D3D12_MEMCPY_DEST DestData = { pData + pLayouts[i].Offset, pLayouts[i].Footprint.RowPitch, SIZE_T(pLayouts[i].Footprint.RowPitch) * SIZE_T(pNumRows[i]) };
				MemcpySubresource(&DestData, &pSrcData[i], static_cast<SIZE_T>(pRowSizesInBytes[i]), pNumRows[i], pLayouts[i].Footprint.Depth);
			}
			pIntermediate->Unmap(0, nullptr);

			if (DestinationDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
			{
				pCmdList->CopyBufferRegion(
					pDestinationResource, 0, pIntermediate, pLayouts[0].Offset, pLayouts[0].Footprint.Width);
			}
			else
			{
				for (UINT i = 0; i < NumSubresources; ++i)
				{
					CD3DX12_TEXTURE_COPY_LOCATION Dst(pDestinationResource, i + FirstSubresource);
					CD3DX12_TEXTURE_COPY_LOCATION Src(pIntermediate, pLayouts[i]);
					pCmdList->CopyTextureRegion(&Dst, 0, 0, 0, &Src, nullptr);
				}
			}
			return RequiredSize;
		}

		//------------------------------------------------------------------------------------------------
		// Heap-allocating UpdateSubresources implementation
		inline UINT64 UpdateSubresources(
			_In_ ID3D12GraphicsCommandList * pCmdList,
			_In_ ID3D12Resource * pDestinationResource,
			_In_ ID3D12Resource * pIntermediate,
			UINT64 IntermediateOffset,
			_In_range_(0, D3D12_REQ_SUBRESOURCES) UINT FirstSubresource,
			_In_range_(0, D3D12_REQ_SUBRESOURCES - FirstSubresource) UINT NumSubresources,
			_In_reads_(NumSubresources) D3D12_SUBRESOURCE_DATA * pSrcData)
		{
			UINT64 RequiredSize = 0;
			UINT64 MemToAlloc = static_cast<UINT64>(sizeof(D3D12_PLACED_SUBRESOURCE_FOOTPRINT) + sizeof(UINT) + sizeof(UINT64)) * NumSubresources;
			if (MemToAlloc > SIZE_MAX)
			{
				return 0;
			}
			void* pMem = HeapAlloc(GetProcessHeap(), 0, static_cast<SIZE_T>(MemToAlloc));
			if (pMem == nullptr)
			{
				return 0;
			}
			auto pLayouts = reinterpret_cast<D3D12_PLACED_SUBRESOURCE_FOOTPRINT*>(pMem);
			UINT64* pRowSizesInBytes = reinterpret_cast<UINT64*>(pLayouts + NumSubresources);
			UINT * pNumRows = reinterpret_cast<UINT*>(pRowSizesInBytes + NumSubresources);

			auto Desc = pDestinationResource->GetDesc();
			ID3D12Device * pDevice = nullptr;
			pDestinationResource->GetDevice(__uuidof(*pDevice), reinterpret_cast<void**>(&pDevice));
			pDevice->GetCopyableFootprints(&Desc, FirstSubresource, NumSubresources, IntermediateOffset, pLayouts, pNumRows, pRowSizesInBytes, &RequiredSize);
			pDevice->Release();

			UINT64 Result = UpdateSubresources(pCmdList, pDestinationResource, pIntermediate, FirstSubresource, NumSubresources, RequiredSize, pLayouts, pNumRows, pRowSizesInBytes, pSrcData);
			HeapFree(GetProcessHeap(), 0, pMem);
			return Result;
		}

		//------------------------------------------------------------------------------------------------
		// Stack-allocating UpdateSubresources implementation
		template <UINT MaxSubresources>
		inline UINT64 UpdateSubresources(
			_In_ ID3D12GraphicsCommandList * pCmdList,
			_In_ ID3D12Resource * pDestinationResource,
			_In_ ID3D12Resource * pIntermediate,
			UINT64 IntermediateOffset,
			_In_range_(0, MaxSubresources) UINT FirstSubresource,
			_In_range_(1, MaxSubresources - FirstSubresource) UINT NumSubresources,
			_In_reads_(NumSubresources) D3D12_SUBRESOURCE_DATA * pSrcData)
		{
			UINT64 RequiredSize = 0;
			D3D12_PLACED_SUBRESOURCE_FOOTPRINT Layouts[MaxSubresources];
			UINT NumRows[MaxSubresources];
			UINT64 RowSizesInBytes[MaxSubresources];

			auto Desc = pDestinationResource->GetDesc();
			ID3D12Device* pDevice = nullptr;
			pDestinationResource->GetDevice(__uuidof(*pDevice), reinterpret_cast<void**>(&pDevice));
			pDevice->GetCopyableFootprints(&Desc, FirstSubresource, NumSubresources, IntermediateOffset, Layouts, NumRows, RowSizesInBytes, &RequiredSize);
			pDevice->Release();

			return UpdateSubresources(pCmdList, pDestinationResource, pIntermediate, FirstSubresource, NumSubresources, RequiredSize, Layouts, NumRows, RowSizesInBytes, pSrcData);
		}
	}

	//----------------------------------------------------------------------------------------------
	void CommandListDX12::resourceBarrier(GpuBuffer* resource, Barrier barrierType, ResourceState before, ResourceState after)
	{
		if(barrierType != Barrier::Transition)
			return;

		auto* dx12Resource = static_cast<GpuBufferDX12*>(resource);

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.StateBefore = dx12StateFromCommon(before);
		barrier.Transition.StateAfter = dx12StateFromCommon(after);
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.pResource = dx12Resource->m_dx12Buffer.Get();

		m_commandList->ResourceBarrier(1, &barrier);
	}

	//----------------------------------------------------------------------------------------------
	void CommandListDX12::clearRenderTarget(RenderTargetView* rt, math::Vec4f color)
	{
		auto* rtdx12 = static_cast<RenderTargetViewDX12*>(rt);
		m_commandList->ClearRenderTargetView(rtdx12->handle, color.data(), 0, nullptr);
	}

	//----------------------------------------------------------------------------------------------
	void CommandListDX12::uploadBufferContent(const GpuBuffer& dst, const GpuBuffer& stagingBuffer, size_t dataSize, const void* data)
	{
		auto& destBufferdx12 = static_cast<const GpuBufferDX12&>(dst);
		auto& stagingBufferdx12 = static_cast<const GpuBufferDX12&>(dst);
		D3D12_SUBRESOURCE_DATA resourceData;
		resourceData.pData = data;
		resourceData.RowPitch = dataSize;
		resourceData.SlicePitch = dataSize;
		UpdateSubresources(m_commandList.Get(), destBufferdx12.m_dx12Buffer.Get(), stagingBufferdx12.m_dx12Buffer.Get(), 0, 0, 0, &resourceData);
	}
}

