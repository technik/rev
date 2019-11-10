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

#include <numeric>

namespace rev::gfx {

	//--------------------------------------------------------------------------
	RenderGraph::RenderGraph(Device& gfxDevice)
		: m_gfxDevice(gfxDevice)
	{}

	//--------------------------------------------------------------------------
	void RenderGraph::reset()
	{
		m_passDescriptors.clear();
		m_bufferLifetime.clear();
		m_sortedPasses.clear();
	}

	//--------------------------------------------------------------------------
	void RenderGraph::addPass(
		const math::Vec2u& size,
		PassDefinition passDefinition,
		PassEvaluator passEvaluator,
		HWAntiAlias targetAntiAliasing)
	{
		// Add a new pass
		m_passDescriptors.emplace_back(m_bufferLifetime);
		auto& newPass = m_passDescriptors.back();
		newPass.targetSize = size;
		newPass.definition = passDefinition;
		newPass.evaluator = passEvaluator;
		newPass.antiAliasing = targetAntiAliasing;
	}

	//--------------------------------------------------------------------------
	void RenderGraph::build()
	{
		assert(m_bufferLifetime.empty());

		// For each pass stored, define graph dependencies by running the pass definition delegate
		for (auto& desc : m_passDescriptors)
		{
			desc.definition(desc);
		}

		// Sort passes based on their dependencies
		sortPasses();

		// Associate passes with resources
		// Resolve input textures?
	}

	//--------------------------------------------------------------------------
	// Sort passes based on their dependencies
	void RenderGraph::sortPasses()
	{
		m_sortedPasses.reserve(m_passDescriptors.size());
		std::iota(m_sortedPasses.begin(), m_sortedPasses.end(), 0);
		// In place sort
		for (size_t i = 0; i < m_sortedPasses.size(); ++i)
		{
			auto currentPassNdx = m_sortedPasses[i];
			const auto& currentPass = m_passDescriptors[currentPassNdx];

			for (size_t j = i + 1; j < m_sortedPasses.size(); ++i)
			{
				bool unsatisfiedDependency = false;

				const auto& laterPass = m_passDescriptors[j];
				// Check no later pass writes to my input dependencies
				for (auto input : currentPass.m_inputs)
				{
					if (std::find(laterPass.m_outputs.begin(), laterPass.m_outputs.end(), input) != laterPass.m_outputs.end())
					{
						unsatisfiedDependency = true;
						break;
					}
				}

				// Check this pass doesn't overwrite other pass's inputs before that pass can read it
				for (auto output : currentPass.m_outputs)
				{
					const PassState& bufferAfterWrite = m_bufferLifetime[output];

					for (auto laterInput : laterPass.m_inputs)
					{
						const auto& laterDependency = m_bufferLifetime[laterInput];
						if (laterDependency.virtualBufferNdx == bufferAfterWrite.virtualBufferNdx
							&& bufferAfterWrite.writeCounter > laterDependency.writeCounter) // This write can't happen yet
						{
							unsatisfiedDependency = true;
							break;
						}
					}

					if (unsatisfiedDependency)
					{
						break;
					}
				}

				// Reorder passes with unsatisfied dependencies
				if (unsatisfiedDependency)
				{
					std::swap(m_sortedPasses[i], m_sortedPasses[j]);
					--i; // Retry with the new order
					break;
				}
			}
		}
	}

	//--------------------------------------------------------------------------
	void RenderGraph::evaluate(CommandBuffer& dst)
	{
		// For each pass in compiled passes
		for (auto passNdx : m_sortedPasses)
		{
			auto& pass = m_passDescriptors[passNdx];
			// Bind target frame buffer
			dst.bindFrameBuffer(pass.m_targetFrameBuffer);
			// Call the evaluator
			pass.evaluator(pass.m_boundInputs, pass.m_inputs.size(), dst);
		}
	}

	//--------------------------------------------------------------------------
	void RenderGraph::clearResources()
	{
		m_physicalBuffers.clear();
	}

	//--------------------------------------------------------------------------
	RenderGraph::BufferResource RenderGraph::PassBuilder::write(FrameBuffer target, BufferFormat targetFormat)
	{
		VirtualBuffer virtualTarget;
		virtualTarget.size = targetSize;
		virtualTarget.format = targetFormat;
		auto prevEntry = std::find(m_buffers.begin(), m_buffers.end(), target);
		if (prevEntry == m_buffers.end())
		{
			virtualTarget.physicslBuffer = m_buffers.size();
			m_buffers.push_back(target);
		}
		else
		{
			virtualTarget.physicslBuffer = prevEntry - m_buffers.begin();
		}

		// TODO: Store virtual buffer
		// TODO: Create a buffer state with the first write of the new virtual buffer
		// TODO: Register this into the outputs of this pass

		return BufferResource(m_buffersState.size() - 1);
	}

	//--------------------------------------------------------------------------
	RenderGraph::BufferResource RenderGraph::PassBuilder::write(RenderGraph::BufferResource target)
	{
		// TODO: Everything but the return value

		return BufferResource(m_buffersState.size() - 1);
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