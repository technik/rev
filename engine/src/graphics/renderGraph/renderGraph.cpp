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
	{
		TextureSampler::Descriptor desc;
		desc.filter = TextureSampler::MinFilter::Linear;
		desc.wrapS = TextureSampler::Wrap::Clamp;
		desc.wrapT = TextureSampler::Wrap::Clamp;
		//m_linearSampler = device.createTextureSampler(desc);
	}

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
			// Is fb allocated already?
			if(!pass.m_targetFB.isValid()) {
				FrameBuffer::Descriptor fbDesc;
				// Create descriptor attachments
				std::vector<FrameBuffer::Attachment> targetAtt;
				bool writesDepth = pass.m_depthOutput.m_afterWrite.isValid();
				targetAtt.resize(pass.m_colorOutputs.size() + writesDepth ? 1 : 0);

				// Fill in attachments
				size_t i = 0;
				for(; i < pass.m_colorOutputs.size(); ++i)
				{
					targetAtt[i].imageType = FrameBuffer::Attachment::Texture;
					targetAtt[i].target = FrameBuffer::Attachment::Color;
					auto attachAlias = pass.m_colorOutputs.at((int)i).m_afterWrite;
					targetAtt[i].texture = m_resolvedTextures.at(attachAlias.id());
				}
				if(writesDepth)
				{
					targetAtt[i].imageType = FrameBuffer::Attachment::Texture;
					targetAtt[i].target = FrameBuffer::Attachment::Depth;
					auto attachAlias = pass.m_depthOutput.m_afterWrite;
					targetAtt[i].texture = m_resolvedTextures.at(attachAlias.id());
				}

				fbDesc.attachments = targetAtt.data();
				fbDesc.numAttachments = targetAtt.size();

				//pass.m_targetFB = m_device.createFrameBuffer(fbDesc);
			}
			// Bind pass resources
			dst.bindFrameBuffer(pass.m_targetFB);
			dst.setViewport(math::Vec2u::zero(), pass.m_targetSize); // viewport
			dst.setScissor(math::Vec2u::zero(), pass.m_targetSize); // clear
			pass.m_execution(*this, dst); // Record pass commands
		}
	}

	//----------------------------------------------------------------------------------------------
	void RenderGraph::run()
	{
		CommandBuffer frameCommands;
		recordExecution(frameCommands);
		//m_device.renderQueue().submitCommandBuffer(frameCommands);
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
		passInfo.m_antiAlias = aa;
		passInfo.m_targetSize = size;
		return newPass;
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
	auto RenderGraph::writeColor(Pass pass, ColorFormat colorFmt, int bindingLocation, ReadMode readMode, Attachment src) -> Attachment
	{
		Attachment afterWrite(m_nextResourceId++);
		WriteAttachment outAttach { readMode, src, afterWrite };
		if(src.isValid()) // Resolve new attachment to previous texture
			m_resolvedTextures.emplace(afterWrite.id(), m_resolvedTextures.at(src.id()));
		else // Create a new texture for the attachment
		{
			Texture2d::Descriptor desc;
			desc.depth = false;
			desc.mipLevels = 1;
			desc.pixelFormat.numChannels = 4;
			desc.pixelFormat.channel = (colorFmt == ColorFormat::RGBA32) ? Image::ChannelFormat::Float32 : Image::ChannelFormat::Byte;
			desc.sRGB = (colorFmt == ColorFormat::sRGBA8);
			desc.providedImages = 0;
			desc.size = m_renderPasses[pass.id()].m_targetSize;
			desc.sampler = m_linearSampler;
			//m_resolvedTextures.emplace(afterWrite.id(), m_device.createTexture2d(desc));
		}
		m_renderPasses[pass.id()].m_colorOutputs.emplace(bindingLocation, outAttach );
		return afterWrite;
	}

	//----------------------------------------------------------------------------------------------
	auto RenderGraph::writeDepth(Pass pass, DepthFormat, ReadMode readMode, Attachment src) -> Attachment
	{
		Attachment afterWrite(m_nextResourceId++);
		WriteAttachment outAttach { readMode, src, afterWrite };
		auto& renderPass = m_renderPasses[pass.id()];
		assert(!renderPass.m_depthOutput.m_afterWrite.isValid()); // We can only write to one depth attachment per pass
		if(src.isValid()) // Resolve new attachment to previous texture
			m_resolvedTextures.emplace(afterWrite.id(), m_resolvedTextures.at(src.id()));
		else // Create a new texture for the attachment
		{
			Texture2d::Descriptor desc;
			desc.depth = true;
			desc.mipLevels = 1;
			desc.pixelFormat.numChannels = 1;
			desc.pixelFormat.channel = Image::ChannelFormat::Float32;
			desc.sRGB = false;
			desc.providedImages = 0;
			desc.size = m_renderPasses[pass.id()].m_targetSize;
			desc.sampler = m_linearSampler;
			//m_resolvedTextures.emplace(afterWrite.id(), m_device.createTexture2d(desc));
		}
		m_renderPasses[pass.id()].m_depthOutput = outAttach;
		return afterWrite;
	}

}