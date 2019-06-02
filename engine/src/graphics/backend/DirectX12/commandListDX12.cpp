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
#include "pipelineDX12.h"
#include "d3dx12.h"

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
	void CommandListDX12::clearDepth(RenderTargetView* depthBufferView, float depth)
	{
		auto* rtdx12 = static_cast<RenderTargetViewDX12*>(depthBufferView);
		m_commandList->ClearDepthStencilView(rtdx12->handle, D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr);
	}

	//----------------------------------------------------------------------------------------------
	void CommandListDX12::bindPipeline(const Pipeline* pipeline)
	{
		auto dx12Pipeline = static_cast<const PipelineDX12*>(pipeline);
		m_commandList->SetPipelineState(dx12Pipeline->m_pipelineState.Get());
		m_commandList->SetGraphicsRootSignature(dx12Pipeline->m_rootSignature.Get());
	}

	//----------------------------------------------------------------------------------------------
	void CommandListDX12::bindAttribute(int binding, int sizeInBytes, int stride, GpuBuffer* attrBuffer, uint32_t offset)
	{
		auto dx12buffer = static_cast<GpuBufferDX12*>(attrBuffer);

		D3D12_VERTEX_BUFFER_VIEW attrDesc;
		attrDesc.BufferLocation = dx12buffer->m_dx12Buffer->GetGPUVirtualAddress() + offset;
		attrDesc.SizeInBytes = sizeInBytes;
		attrDesc.StrideInBytes = stride;

		m_commandList->IASetVertexBuffers(binding, 1, &attrDesc);
	}

	//----------------------------------------------------------------------------------------------
	void CommandListDX12::bindIndexBuffer(int sizeInBytes, NdxBufferFormat ndxType, GpuBuffer* attrBuffer, uint32_t offset)
	{
		auto dx12buffer = static_cast<GpuBufferDX12*>(attrBuffer);

		D3D12_INDEX_BUFFER_VIEW IndexBufferView;
		IndexBufferView.BufferLocation = dx12buffer->m_dx12Buffer->GetGPUVirtualAddress() + offset;
		IndexBufferView.SizeInBytes = sizeInBytes;
		switch (ndxType)
		{
		case NdxBufferFormat::U8:
			IndexBufferView.Format = DXGI_FORMAT_R8_UINT;
			break;
		case NdxBufferFormat::U16:
			IndexBufferView.Format = DXGI_FORMAT_R16_UINT;
			break;
		case NdxBufferFormat::U32:
			IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
			break;
		default:
			assert(false);
			break;
		}

		m_commandList->IASetIndexBuffer(&IndexBufferView);
	}

	//----------------------------------------------------------------------------------------------
	void CommandListDX12::bindRenderTarget(RenderTargetView* color, RenderTargetView* depth)
	{
		auto rt12 = static_cast<RenderTargetViewDX12*>(color);
		auto depthDx12 = static_cast<RenderTargetViewDX12*>(depth);

		m_commandList->OMSetRenderTargets(1, &rt12->handle, false, depth ? &depthDx12->handle : nullptr);
	}

	//----------------------------------------------------------------------------------------------
	void CommandListDX12::drawIndexed(int indexOffset, int indexCount, int vertexOffset)
	{
		m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_commandList->DrawIndexedInstanced(indexCount, 1, indexOffset, vertexOffset, 0);
	}

	//----------------------------------------------------------------------------------------------
	void CommandListDX12::setViewport(const math::Vec2u& pos, const math::Vec2u& size)
	{
		D3D12_VIEWPORT viewport;
		viewport.TopLeftX = (FLOAT)pos.x();
		viewport.TopLeftY = (FLOAT)pos.y();
		viewport.Width = (FLOAT)size.x();
		viewport.Height = (FLOAT)size.y();
		// Min,max ranges are [0,1]
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;
		m_commandList->RSSetViewports(1, &viewport);
	}

	//----------------------------------------------------------------------------------------------
	void CommandListDX12::setScissor(const math::Vec2u& pos, const math::Vec2u& size)
	{
		D3D12_RECT scissor;
		scissor.left = pos.x();
		scissor.right = pos.x() + size.x();
		scissor.top = pos.y();
		scissor.bottom = pos.y() + size.y();
		m_commandList->RSSetScissorRects(1, &scissor);
	}

	//----------------------------------------------------------------------------------------------
	void CommandListDX12::setConstants(uint32_t regIndex0, uint32_t size, const void* data)
	{
		m_commandList->SetGraphicsRoot32BitConstants(regIndex0, size / 4, data, 0);
	}

	//----------------------------------------------------------------------------------------------
	void CommandListDX12::uploadBufferContent(const GpuBuffer& dst, const GpuBuffer& stagingBuffer, size_t dataSize, const void* data)
	{
		auto& destBufferdx12 = static_cast<const GpuBufferDX12&>(dst);
		auto& stagingBufferdx12 = static_cast<const GpuBufferDX12&>(stagingBuffer);
		D3D12_SUBRESOURCE_DATA resourceData = {};
		resourceData.pData = data;
		resourceData.RowPitch = dataSize;
		resourceData.SlicePitch = dataSize;
		UpdateSubresources(m_commandList.Get(), destBufferdx12.m_dx12Buffer.Get(), stagingBufferdx12.m_dx12Buffer.Get(), 0, 0, 1, &resourceData);
	}
}

