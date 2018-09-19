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
	void RenderPassOpenGL::reset()
	{
		m_commandList.clear();
	}

	//----------------------------------------------------------------------------------------------
	void RenderPassOpenGL::setViewport(const math::Vec2u& start, const math::Vec2u& size)
	{
		m_viewportStart = start;
		m_viewportSize = size;
	}

	//----------------------------------------------------------------------------------------------
	void RenderPassOpenGL::record(const CommandBuffer& cmdBuffer)
	{
		m_commandList.push_back(&cmdBuffer);
	}

	//----------------------------------------------------------------------------------------------
	void RenderPassOpenGL::submit(RenderQueueOpenGL& renderQueue) const
	{
		// Bind the frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, m_desc.target.id);

		// Config viewport
		glViewport(m_viewportStart.x(), m_viewportStart.y(), m_viewportSize.x(), m_viewportSize.y());
		glScissor(m_viewportStart.x(), m_viewportStart.y(), m_viewportSize.x(), m_viewportSize.y());

		// Clear
		if((int)m_desc.clearFlags & (int)Descriptor::Clear::Color)
		{
			glClearColor(m_desc.clearColor.x(),m_desc.clearColor.y(),m_desc.clearColor.z(),m_desc.clearColor.w());
		}
		if((int)m_desc.clearFlags & (int)Descriptor::Clear::Depth)
		{
			glClearDepthf(m_desc.clearDepth);
		}
		if((int)m_desc.clearFlags)
		{
			glClear(
				((int)Descriptor::Clear::Color ? GL_COLOR_BUFFER_BIT : 0)
				|((int)Descriptor::Clear::Depth ? GL_DEPTH_BUFFER_BIT : 0));
		}

		// Send recorded commands
		for(auto cmdBuffer : m_commandList)
			renderQueue.submitCommandBuffer(*cmdBuffer);
	}
}
