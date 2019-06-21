//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2018 Carmelo J Fdez-Aguera
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
#include "renderGraph.h"

namespace rev::gfx {

	//--------------------------------------------------------------------------------------------------
	void RenderGraph::addPass(std::string_view passName, const PassConstructionCb& constructCb, const PassExecutionCb& execCb)
	{
		m_passes[m_numPasses++] = { std::string(passName), constructCb, execCb };
	}

	//--------------------------------------------------------------------------------------------------
	auto RenderGraph::createDepthRT(const math::Vec2u& size) -> DepthRT
	{
		assert(false && "Not implemented");
		DepthRT rt{ m_numBuffers++ };
		return rt;
	}

	//--------------------------------------------------------------------------------------------------
	auto RenderGraph::importRT(const math::Vec2u& size, GpuBuffer& buffer, RenderTargetView& rtv, CommandList::ResourceState defaultState) -> ColorRT
	{
		auto& resource = m_gpuBuffers[m_numBuffers];
		resource.size = size;
		resource.gpuBuffer = &buffer;
		resource.rtv = &rtv;
		ColorRT resourceId{ m_numBuffers++ };
		return resourceId;
	}

	//--------------------------------------------------------------------------------------------------
	void RenderGraph::reset()
	{
		assert(false && "Not implemented. Should destroy resources here");
	}

	//--------------------------------------------------------------------------------------------------
	void RenderGraph::compile()
	{
		PassBuilderImpl builder;
		for(unsigned i = 0; i < m_numPasses; ++i)
		{ 
			auto& pass = m_passes[i];
			pass.clearZ = false;
			pass.clearColor = false;
			builder.currentPass = &m_passes[i];
			pass.constructionCb(builder);
		}
	}

	//--------------------------------------------------------------------------------------------------
	void RenderGraph::record(CommandList& cmdList)
	{
		for (unsigned i = 0; i < m_numPasses; ++i)
		{
			auto& pass = m_passes[i];
			auto& colorAttach = m_gpuBuffers[pass.colorAttach];
			auto* depthAttach = pass.depthAttach == uint32_t(-1) ? nullptr : &m_gpuBuffers[pass.depthAttach];
			// Need clear?
			if(depthAttach && pass.clearZ)
			{ 
				cmdList.clearDepth(depthAttach->rtv, pass.zValue);
			}
			if(pass.clearColor)
			{
				cmdList.clearRenderTarget(colorAttach.rtv, pass.color);
			}
			// Need binding?
			pass.executionCb(cmdList);
		}
	}
}