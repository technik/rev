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
#include <graphics/backend/commandBuffer.h>
#include <graphics/backend/device.h>

namespace rev::gfx {

	//--------------------------------------------------------------------------
	RenderGraph::RenderGraph(Device& gfxDevice)
		: m_gfxDevice(gfxDevice)
	{}

	//--------------------------------------------------------------------------
	void RenderGraph::reset()
	{
		m_passDescriptors.clear();
	}

	//--------------------------------------------------------------------------
	void RenderGraph::addPass(
		const math::Vec2u& size,
		PassDefinition passDefinition,
		PassEvaluator passEvaluator,
		HWAntiAlias targetAntiAliasing)
	{
	}

	//--------------------------------------------------------------------------
	void RenderGraph::build()
	{
		m_buffersState.clear();
		// For each pass stored
		for (const auto& desc : m_passDescriptors)
		{
			PassBuilder builder { m_buffersState };
			builder.targetSize = desc.targetSize;
			desc.definition(builder);
			// Allocate any new buffers needed by the builder?
		}
	}

	//--------------------------------------------------------------------------
	void RenderGraph::evaluate(CommandBuffer& dst)
	{
		// For each pass in compiled passes
		// Bind targets
		// Expose read buffers as an input structure to the pass evaluator
		// Call the evaluator
	}

	//--------------------------------------------------------------------------
	void RenderGraph::clearResources()
	{
	}

	//--------------------------------------------------------------------------
	RenderGraph::BufferResource RenderGraph::PassBuilder::write(FrameBuffer target)
	{
		
		return BufferResource(0);
	}

	//--------------------------------------------------------------------------
	RenderGraph::BufferResource RenderGraph::PassBuilder::write(DepthFormat target)
	{
		return BufferResource(0);
	}

	//--------------------------------------------------------------------------
	RenderGraph::BufferResource RenderGraph::PassBuilder::write(ColorFormat target)
	{
		return BufferResource(0);
	}

	//--------------------------------------------------------------------------
	RenderGraph::BufferResource RenderGraph::PassBuilder::write(RenderGraph::BufferResource target)
	{
		return BufferResource(0);
	}

	//--------------------------------------------------------------------------
	void RenderGraph::PassBuilder::read(RenderGraph::BufferResource target, int bindingPos)
	{
	}

	//--------------------------------------------------------------------------
	void RenderGraph::PassBuilder::modify(RenderGraph::BufferResource target)
	{
	}
}