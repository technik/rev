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

	//----------------------------------------------------------------------------------------------
	RenderGraph::RenderGraph(Device& device)
		: m_device(device)
	{}

	//----------------------------------------------------------------------------------------------
	void RenderGraph::reset()
	{
		m_nextResourceId = 0;
		m_renderPasses.clear();
	}

	//----------------------------------------------------------------------------------------------
	void RenderGraph::compile()
	{
		// TODO. For now, just execute passes linearly as they were recorded
	}

	//----------------------------------------------------------------------------------------------
	void RenderGraph::recordExecution(CommandBuffer& dst)
	{
		for(auto& pass : m_renderPasses)
		{
			// Bind pass resources
			// fb
			// viewport
			// clear
			pass.m_execution(dst); // Record pass commands
		}
	}

	//----------------------------------------------------------------------------------------------
	void RenderGraph::run()
	{
		CommandBuffer frameCommands;
		recordExecution(frameCommands);
		m_device.renderQueue().submitCommandBuffer(frameCommands);
	}

	//----------------------------------------------------------------------------------------------
	void RenderGraph::clearResources()
	{}

	//----------------------------------------------------------------------------------------------
	auto RenderGraph::pass(const math::Vec2u& size, HWAntiAlias aa) -> Pass
	{
		Pass newPass (m_renderPasses.size());
		m_renderPasses.emplace_back();
		auto& passInfo = m_renderPasses.back();
		passInfo.m_antiAlias == aa;
		passInfo.m_targetSize = size;
	}

	//----------------------------------------------------------------------------------------------
	auto RenderGraph::pass(const math::Vec2u& size, HWAntiAlias aa) -> Pass
	{
		Pass newPass (m_renderPasses.size());
		m_renderPasses.emplace_back();
		auto& passInfo = m_renderPasses.back();
		passInfo.m_antiAlias == aa;
		passInfo.m_targetSize = size;
	}

	//----------------------------------------------------------------------------------------------
	void RenderGraph::readColor(Pass pass, int bindingLocation, Attachment src)
	{
		assert(src.isValid());
		m_renderPasses[pass.id()].m_colorInputs.emplace(bindingLocation, src);
	}

	//----------------------------------------------------------------------------------------------
	void RenderGraph::readDepth(Pass pass, int bindingLocation, Attachment src)
	{
		assert(src.isValid());
		m_renderPasses[pass.id()].m_depthInputs.emplace(bindingLocation, src);
	}

	//----------------------------------------------------------------------------------------------
	auto RenderGraph::writeColor(Pass pass, int bindingLocation, ReadMode readMode, Attachment src) -> Attachment
	{
		Attachment afterWrite(m_nextResourceId++);
		WriteAttachment outAttach { readMode, src, afterWrite };
		m_renderPasses[pass.id()].m_colorOutputs.emplace(bindingLocation, outAttach );
		return afterWrite;
	}

	//----------------------------------------------------------------------------------------------
	auto RenderGraph::writeDepth(Pass pass, ReadMode readMode, Attachment src) -> Attachment
	{
		Attachment afterWrite(m_nextResourceId++);
		WriteAttachment outAttach { readMode, src, afterWrite };
		auto& renderPass = m_renderPasses[pass.id()];
		assert(!renderPass.m_depthOutput.m_afterWrite.isValid()); // We can only write to one depth attachment per pass
		m_renderPasses[pass.id()].m_depthOutput = outAttach;
		return afterWrite;
	}

}