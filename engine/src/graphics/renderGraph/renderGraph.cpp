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
#include "frameBufferCache.h"
#include "renderGraph.h"
#include <graphics/backend/commandBuffer.h>
#include <graphics/backend/device.h>
#include <graphics/debug/debugGUI.h>
#include <graphics/debug/imgui.h>

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
		const std::string& name,
		const math::Vec2u& size,
		PassDefinition passDefinition,
		PassEvaluator passEvaluator,
		HWAntiAlias targetAntiAliasing)
	{
		// Add a new pass
		m_passDescriptors.emplace_back(name, m_bufferLifetime, m_virtualResources);
		auto& newPass = m_passDescriptors.back();
		newPass.targetSize = size;
		newPass.definition = passDefinition;
		newPass.evaluator = passEvaluator;
		newPass.antiAliasing = targetAntiAliasing;
	}

	//--------------------------------------------------------------------------
	void RenderGraph::build(FrameBufferCache& bufferCache)
	{
		assert(m_bufferLifetime.empty());

		// For each pass stored, define graph dependencies by running the pass definition delegate
		for (auto& desc : m_passDescriptors)
		{
			desc.definition(desc);
		}

		// Sort passes based on their dependencies
		sortPasses();

		bool showDebugInfo = ImGui::Begin("Render Graph");

		// Clear previous associations
		m_virtualToPhysical.clear();
		// Associate passes with resources
		for (auto passNdx : m_sortedPasses)
		{
			FrameBuffer passFramebuffer;
			auto& pass = m_passDescriptors[passNdx];
			// Gather all virtual resources associated
			Texture2d targetTextures[cMaxOutputs];
			Texture2d::CubeMapSide targetTextureSide = Texture2d::CubeMapSide::None;
			for (int i = 0; i < pass.m_outputs.size(); ++i)
			{
				auto outputStateNdx = pass.m_outputs[i];
				auto targetResourceNdx = m_bufferLifetime[outputStateNdx].virtualBufferNdx;
				auto& virtualBuffer = m_virtualResources[targetResourceNdx];
				if (virtualBuffer.externalFramebuffer.isValid())
				{
					passFramebuffer = virtualBuffer.externalFramebuffer;
					break;
				}

				auto virtualIter = m_virtualToPhysical.find(targetResourceNdx);
				if (virtualIter == m_virtualToPhysical.end()) // Resource not previously mapped
				{
					if (virtualBuffer.externalTexture.isValid())
					{
						targetTextures[i] = virtualBuffer.externalTexture;
						targetTextureSide = virtualBuffer.cubemapSide;
					}
					else
					{
						targetTextures[i] = bufferCache.requestTargetTexture(virtualBuffer.bufferDescriptor);
					}
					m_virtualToPhysical[targetResourceNdx] = targetTextures[i];
				}
				else // Previously mapped resource
				{
					targetTextures[i] = virtualIter->second;
				}
			}
			if (passFramebuffer.isValid())
			{
				pass.m_cachedFb = passFramebuffer;
				continue;
			}

			// Agregate target textures into a framebuffer descriptor
			FrameBuffer::Attachment attachments[cMaxOutputs];
			size_t numAttachs = pass.m_outputs.size();
			bool hasDepthBuffer = false;
			for (int j = 0; j < numAttachs; ++j)
			{
				attachments[j].mipLevel = 0;
				attachments[j].imageType = FrameBuffer::Attachment::ImageType::Texture;
				attachments[j].side = targetTextureSide;
				auto lifeTimeNdx = pass.m_outputs[j];
				auto virtualIndex = m_bufferLifetime[lifeTimeNdx].virtualBufferNdx;
				auto bufferFormat = m_virtualResources[virtualIndex].bufferDescriptor.format;
				attachments[j].target = (bufferFormat == BufferFormat::depth24 || bufferFormat == BufferFormat::depth32) ?
					FrameBuffer::Attachment::Target::Depth : FrameBuffer::Attachment::Target::Color;
				if (attachments[j].target == FrameBuffer::Attachment::Target::Depth)
				{
					assert(!hasDepthBuffer && "Only one depth buffer can be attached per pass");
					hasDepthBuffer = true;
				}
				attachments[j].texture = targetTextures[j];
			}
			// Optionally bind a read-only depth buffer
			if (!hasDepthBuffer)
			{
				for (auto& input : pass.m_inputs)
				{
					auto virtualIndex = m_bufferLifetime[input].virtualBufferNdx;
					auto bufferFormat = m_virtualResources[virtualIndex].bufferDescriptor.format;
					if ((bufferFormat == BufferFormat::depth24 || bufferFormat == BufferFormat::depth32))
					{
						attachments[numAttachs].mipLevel = 0;
						attachments[numAttachs].imageType = FrameBuffer::Attachment::ImageType::Texture;
						attachments[numAttachs].target = FrameBuffer::Attachment::Target::Depth;
						assert(m_virtualToPhysical.find(virtualIndex) != m_virtualToPhysical.end() && "Depth must always be written before beind used as read only");
						attachments[numAttachs].texture = m_virtualToPhysical[virtualIndex];

						++numAttachs;
						break;
					}
				}
			}
			FrameBuffer::Descriptor descriptor(numAttachs, attachments);
			passFramebuffer = bufferCache.requestFrameBuffer(descriptor);
			pass.m_cachedFb = passFramebuffer;

			// Instrumentation
			if(ImGui::CollapsingHeader(pass.name.c_str()))
			{
				ImVec2 previewSize = ImVec2(pass.targetSize.x() * 0.25f, pass.targetSize.y() * 0.25f);
				for (int nAttach = 0; nAttach < pass.m_outputs.size(); ++nAttach)
				{
					ImTextureID texId = (void*)((GLuint)attachments[nAttach].texture.id());
					ImGui::Image(texId, previewSize);
				}
			}
		}

		ImGui::End();

		// Resolve input textures?
		bufferCache.freeResources();
	}

	//--------------------------------------------------------------------------
	// Sort passes based on their dependencies
	void RenderGraph::sortPasses()
	{
		m_sortedPasses.resize(m_passDescriptors.size());
		std::iota(m_sortedPasses.begin(), m_sortedPasses.end(), 0);
		// In place sort
		for (size_t i = 0; i < m_sortedPasses.size(); ++i)
		{
			auto currentPassNdx = m_sortedPasses[i];
			const auto& currentPass = m_passDescriptors[currentPassNdx];

			for (size_t j = i + 1; j < m_sortedPasses.size(); ++j)
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
			dst.bindFrameBuffer(pass.m_cachedFb);
			dst.setViewport(math::Vec2u::zero(), pass.targetSize);
			dst.setScissor(math::Vec2u::zero(), pass.targetSize);
			// Collapse input textures into a local array of physical textures
			Texture2d passInputs[PassBuilder::cMaxInputs];
			assert(pass.m_inputs.size() <= IPassBuilder::cMaxInputs);
			for (size_t i = 0; i < pass.m_inputs.size(); ++i)
			{
				auto& bufferState = m_bufferLifetime[pass.m_inputs[i]];
				passInputs[i] = m_virtualToPhysical[bufferState.virtualBufferNdx].id();
			}
			// Call the evaluator
			pass.evaluator(passInputs, pass.m_inputs.size(), dst);
		}
	}

	//--------------------------------------------------------------------------
	void RenderGraph::clearResources()
	{
	}

	//--------------------------------------------------------------------------
	RenderGraph::BufferResource RenderGraph::PassBuilder::write(FrameBuffer target)
	{
		assert(target.isValid());

		// Register target into a virtual frame buffer
		auto bufferNdx = m_virtualResources.size();
		VirtualResource virtualTarget;
		virtualTarget.externalFramebuffer = target;
		virtualTarget.bufferDescriptor.size = targetSize;
		virtualTarget.bufferDescriptor.antiAlias = antiAliasing;
		m_virtualResources.push_back(virtualTarget);

		PassState outputAfterWrite;
		outputAfterWrite.virtualBufferNdx = bufferNdx;
		outputAfterWrite.writeCounter = 1;

		return registerOutput(outputAfterWrite);
	}

	//--------------------------------------------------------------------------
	RenderGraph::BufferResource RenderGraph::PassBuilder::write(Texture2d externalTexture)
	{
		assert(externalTexture.isValid());

		// Register target into a virtual frame buffer
		auto bufferNdx = m_virtualResources.size();
		VirtualResource virtualTarget;
		virtualTarget.externalTexture = externalTexture;
		virtualTarget.bufferDescriptor.size = targetSize;
		virtualTarget.bufferDescriptor.antiAlias = antiAliasing;
		m_virtualResources.push_back(virtualTarget);

		PassState outputAfterWrite;
		outputAfterWrite.virtualBufferNdx = bufferNdx;
		outputAfterWrite.writeCounter = 1;

		return registerOutput(outputAfterWrite);
	}

	//--------------------------------------------------------------------------
	RenderGraph::BufferResource RenderGraph::PassBuilder::write(Texture2d externalTexture, Texture2d::CubeMapSide targetSide)
	{
		assert(externalTexture.isValid());

		// Register target into a virtual frame buffer
		auto bufferNdx = m_virtualResources.size();
		VirtualResource virtualTarget;
		virtualTarget.externalTexture = externalTexture;
		virtualTarget.cubemapSide = targetSide;
		virtualTarget.bufferDescriptor.size = targetSize;
		virtualTarget.bufferDescriptor.antiAlias = antiAliasing;
		m_virtualResources.push_back(virtualTarget);

		PassState outputAfterWrite;
		outputAfterWrite.virtualBufferNdx = bufferNdx;
		outputAfterWrite.writeCounter = 1;

		return registerOutput(outputAfterWrite);
	}

	//--------------------------------------------------------------------------
	RenderGraph::BufferResource RenderGraph::PassBuilder::write(BufferFormat targetFormat)
	{
		// Register a new virtual frame buffer with the requested format
		auto bufferNdx = m_virtualResources.size();
		VirtualResource virtualTarget;
		virtualTarget.bufferDescriptor.format = targetFormat;
		virtualTarget.bufferDescriptor.size = targetSize;
		virtualTarget.bufferDescriptor.antiAlias = antiAliasing;
		m_virtualResources.push_back(virtualTarget);

		PassState outputAfterWrite;
		outputAfterWrite.virtualBufferNdx = bufferNdx;
		outputAfterWrite.writeCounter = 1;

		return registerOutput(outputAfterWrite);
	}

	//--------------------------------------------------------------------------
	RenderGraph::BufferResource RenderGraph::PassBuilder::write(RenderGraph::BufferResource target)
	{
		assert(target.isValid());
		size_t stateNdx = target.id();

		// Taking a valid resource as an argument means some other pass already wrote to this target.
		// The new state just needs to reflect that by increasing the write counter
		auto resultingState = m_bufferLifetime[stateNdx];
		assert(resultingState.writeCounter > 0);
		resultingState.writeCounter++;

		return registerOutput(resultingState);
	}

	//--------------------------------------------------------------------------
	void RenderGraph::PassBuilder::read(RenderGraph::BufferResource target, int bindingPos)
	{
		assert(target.isValid());
		m_inputs.push_back(target.id());
	}

	//--------------------------------------------------------------------------
	RenderGraph::BufferResource RenderGraph::PassBuilder::registerOutput(PassState newOutputState)
	{
		auto newOutputStateNdx = m_bufferLifetime.size();
		m_bufferLifetime.push_back(newOutputState);

		m_outputs.push_back(newOutputStateNdx);
		return BufferResource((int)newOutputStateNdx);
	}
}