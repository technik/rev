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
#pragma once

#include "../renderPass.h"
#include "openGL.h"

namespace rev :: gfx
{
	class RenderPassOpenGL : public RenderPass
	{
	public:
		RenderPassOpenGL(const Descriptor& desc)
			: m_desc(desc)
		{}

		void reset() override {}

		void begin() override {}
		void end() override {}

		// OpenGL specific
		void submit() const
		{
			if((int)m_desc.clearFlags & (int)Descriptor::Clear::Color)
			{
				glClearColor(m_desc.clearColor.x(),m_desc.clearColor.y(),m_desc.clearColor.z(),m_desc.clearColor.w());
			}
			if((int)m_desc.clearFlags & (int)Descriptor::Clear::Z)
			{
				glClearDepthf(m_desc.clearDepth);
			}
			if((int)m_desc.clearFlags)
			{
				glClear(
					((int)Descriptor::Clear::Color ? GL_COLOR_BUFFER_BIT : 0)
					|((int)Descriptor::Clear::Z ? GL_DEPTH_BUFFER_BIT : 0));
			}
		}

	private:
		Descriptor m_desc;
	};
}
