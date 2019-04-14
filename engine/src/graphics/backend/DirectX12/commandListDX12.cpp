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
}

