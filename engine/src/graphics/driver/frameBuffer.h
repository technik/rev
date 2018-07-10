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

#include "texture.h"
#include "renderTarget.h"
#include "openGL/openGL.h"

namespace rev { namespace graphics {

	class FrameBuffer : public RenderTarget
	{
	public:
		FrameBuffer(const math::Vec2u& _size)
			: RenderTarget(_size)
		{
			// Create depth map
			m_texture = Texture::depthTexture(_size);

			// Create framebuffer
			glGenFramebuffers(1, &mFbName);
			glBindFramebuffer(GL_FRAMEBUFFER, mFbName);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_texture->glName(), 0);
			glDrawBuffers(0, nullptr);
			glReadBuffer(GL_NONE);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void bind() const override
		{
			glBindFramebuffer(GL_FRAMEBUFFER, mFbName);
            glDisable(GL_FRAMEBUFFER_SRGB);
			glViewport(0, 0, mSize.x(), mSize.y());
		}

		auto texture() const { return m_texture; }

	private:
		GLuint mFbName;
		std::shared_ptr<Texture> m_texture;
	};

}}
