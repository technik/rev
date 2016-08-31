//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Simple deferred renderer
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_TYPES_RENDERTARGET_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_TYPES_RENDERTARGET_H_

#ifdef _WIN32
#include <Windows.h>
#include <video/graphics/driver/openGL/glew.h>
#else
#include <GL/glew.h>
#endif // !_WIN32

#include <GL/gl.h>

#include <video/basicTypes/texture.h>

namespace rev {
	namespace video {

		class RenderTarget {
		public:
			RenderTarget() {
				glGenFramebuffers(1, &mId);
			}

			~RenderTarget() {
				glDeleteFramebuffers(1, &mId);
			}

			void setTargetColor(Texture* _color) {
				glBindFramebuffer(GL_FRAMEBUFFER, mId);
				GLenum target = _color->multiSample() ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

				mColor = _color;
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, _color->glId(), 0);
			}

			void setTargetDepth(Texture* _depth) {
				glBindFramebuffer(GL_FRAMEBUFFER, mId);
				GLenum target = _depth->multiSample() ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

				mDepth = _depth;
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, target, _depth->glId(), 0);
			}

			unsigned glId() const { return mId; }

			Texture* color() const { return mColor; }
			Texture* depth() const { return mDepth; }

		private:
			Texture* mColor = nullptr;
			Texture* mDepth = nullptr;

			// OpenGL specific
			unsigned mId;
		};
	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_TYPES_RENDERTARGET_H_
