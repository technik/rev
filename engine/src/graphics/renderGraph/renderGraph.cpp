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
		m_buffersState.clear();
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
		m_passDescriptors.emplace_back(m_buffersState, m_physicalBuffers);
		auto& newPass = m_passDescriptors.back();
		newPass.targetSize = size;
		newPass.definition = passDefinition;
		newPass.evaluator = passEvaluator;
		newPass.antiAliasing = targetAntiAliasing;
	}

	//--------------------------------------------------------------------------
	void RenderGraph::build()
	{
		assert(m_buffersState.empty());

		// For each pass stored
		for (const auto& desc : m_passDescriptors)
		{
			PassBuilder builder { m_buffersState, m_physicalBuffers };
			builder.targetSize = desc.targetSize;
			desc.definition(builder);
		}

		// Sort passes based on their dependencies
		m_sortedPasses.reserve(m_passDescriptors.size());

		// Temporary storage for non-sorted passes
		std::vector<size_t> unsortedPasses(m_passDescriptors.size());
		std::iota(unsortedPasses.begin(), unsortedPasses.end(), 0);
		while (!unsortedPasses.empty())
		{
			const auto initialNumberOfPasses = unsortedPasses.size(); // Keep this so we can verify that the number of unsorted passes always decreases

			for (size_t i= 0; i< unsortedPasses.size(); ++i)
			{
				auto targetPassNdx = unsortedPasses[i];

				// Skip passes with unsatisfied dependencies
				bool unsatisfiedDependency = false;
				auto& pass = m_passDescriptors[targetPassNdx];

				for (auto dependency : pass.m_inputs)
				{
					bool satisfied = false;
					for (auto prevPassNdx : m_sortedPasses)
					{
						auto& prevPass = m_passDescriptors[prevPassNdx];
						if (std::find(prevPass.m_outputs.begin(), prevPass.m_outputs.end(), dependency) != prevPass.m_outputs.end())
						{
							satisfied = true;
							break;
						}
					}
					// A single unsatisfied dependency is enough to push the pass to later evaluation
					if (!satisfied)
					{
						unsatisfiedDependency = true;
						break;
					}
				}
				if (unsatisfiedDependency)
					continue;

				// Skip passes that would overwrite a target that will still be needed by a non sorted pass
				bool prematureWrite = false;
				for (auto output : pass.m_outputs)
				{
					auto& outputBufferState = m_buffersState[output];
					auto outputBuffer = outputBufferState.first;
					auto outputWriteCounter = outputBufferState.second;

					for (auto laterPassNdx : unsortedPasses)
					{
						if (laterPassNdx == targetPassNdx)
							continue; // Do not test against self

						auto& laterPass = m_passDescriptors[laterPassNdx];
						for (auto laterInput : laterPass.m_inputs)
						{
							auto& laterPassDependency = m_buffersState[laterInput];
							if (laterPassDependency.first == outputBuffer && laterPassDependency.second < outputWriteCounter)
							{
								prematureWrite = true;
								break;
							}
						}
						if (prematureWrite == true)
							break;
					}
					if (prematureWrite)
						break;
				}
				if (prematureWrite)
					continue;

				// Pass sorted
				m_sortedPasses.push_back(targetPassNdx);

				// In place removal from unsorted passes
				unsortedPasses[i] = unsortedPasses.back();
				unsortedPasses.resize(unsortedPasses.size() - 1);
				--i;
			}

			const auto finalNumberOfPasses = unsortedPasses.size(); // Keep this so we can verify that the number of unsorted passes always decreases
			assert(finalNumberOfPasses < initialNumberOfPasses);
		}

		// Allocate required buffers
		// Resolve input textures
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