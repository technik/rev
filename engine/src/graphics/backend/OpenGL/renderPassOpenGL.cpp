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
#include "renderPassOpenGL.h"
#include "renderQueueOpenGL.h"

namespace rev :: gfx
{
	//----------------------------------------------------------------------------------------------
	RenderPassOpenGL::RenderPassOpenGL(const Descriptor& desc, int numDrawBuffers, int32_t id)
		: RenderPass(id)
		, m_desc(desc)
	{
		if(desc.target.id() == 0)
		{
			m_colorAttachs.push_back(GL_BACK_LEFT);
		}
		else
		{
			m_colorAttachs.resize(numDrawBuffers);
			for(int i = 0; i < numDrawBuffers; ++i)
				m_colorAttachs[i] = GL_COLOR_ATTACHMENT0+i;
		}
	}

	//----------------------------------------------------------------------------------------------
	void RenderPassOpenGL::reset()
	{
		m_commandList.clear();
	}

	//----------------------------------------------------------------------------------------------
	void RenderPassOpenGL::setViewport(const math::Vec2u& start, const math::Vec2u& size)
	{
		m_desc.viewportStart = start;
		m_desc.viewportSize = size;
	}

	//----------------------------------------------------------------------------------------------
	void RenderPassOpenGL::record(const CommandBuffer& cmdBuffer)
	{
		m_commandList.push_back(&cmdBuffer);
	}

	//----------------------------------------------------------------------------------------------
	void RenderPassOpenGL::bindTo(RenderQueueOpenGL& renderQueue) const
	{
		// Bind the frame buffer
		assert(m_desc.target.isValid());
		glBindFramebuffer(GL_FRAMEBUFFER, m_desc.target.id());
		glDrawBuffers(m_colorAttachs.size(), m_colorAttachs.data());
		if(m_desc.sRGB)
			glEnable(GL_FRAMEBUFFER_SRGB);

		// Config viewport
		glViewport(m_desc.viewportStart.x(), m_desc.viewportStart.y(), m_desc.viewportSize.x(), m_desc.viewportSize.y());
		glScissor(m_desc.viewportStart.x(), m_desc.viewportStart.y(), m_desc.viewportSize.x(), m_desc.viewportSize.y());

		// Clear
		if ((int)m_desc.clearFlags & (int)Clear::Color)
		{
			glClearColor(m_desc.clearColor.x(), m_desc.clearColor.y(), m_desc.clearColor.z(), m_desc.clearColor.w());
		}
		if ((int)m_desc.clearFlags & (int)Clear::Depth)
		{
			glClearDepthf(m_desc.clearDepth);
		}
		if ((int)m_desc.clearFlags)
		{
			glClear(
				((int)Clear::Color ? GL_COLOR_BUFFER_BIT : 0)
				| ((int)Clear::Depth ? GL_DEPTH_BUFFER_BIT : 0));
		}
	}

	//----------------------------------------------------------------------------------------------
	void RenderPassOpenGL::submit(RenderQueueOpenGL& renderQueue) const
	{
		// Bind the frame buffer
		bindTo(renderQueue);

		// Send recorded commands
		for(auto cmdBuffer : m_commandList)
			renderQueue.submitCommandBuffer(*cmdBuffer);
	}
}
