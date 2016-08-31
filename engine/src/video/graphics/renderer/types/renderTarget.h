//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Simple deferred renderer
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_TYPES_RENDERTARGET_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_TYPES_RENDERTARGET_H_

namespace rev {
	namespace video {

		class Texture;

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

				color = _color;
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, _color->glId(), 0);
			}

			void setTargetDepth(Texture* _depth) {
				glBindFramebuffer(GL_FRAMEBUFFER, mId);
				GLenum target = _depth->multiSample() ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

				depth = _depth;
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, target, _depth->glId(), 0);
			}

			unsigned glId() const { return mId; }

		private:
			Texture* color = nullptr;
			Texture* depth = nullptr;

			// OpenGL specific
			unsigned mId;
		};
	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_TYPES_RENDERTARGET_H_
