//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_TYPES_FRAMEBUFFERRT_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_TYPES_FRAMEBUFFERRT_H_

#include "renderTarget.h"

namespace rev {
	namespace video {

		class FrameBufferRT : public RenderTarget {
		public:
			float aspectRatio() const = 0;
			math::Vec2u size() const { return ; }
			unsigned glId() const { return 0; }


			/*RenderTarget() {
			glGenFramebuffers(1, &mId);
			}

			~RenderTarget() {
			glDeleteFramebuffers(1, &mId);
			}

			void setTargetColor(Texture* _color) {
			glBindFramebuffer(GL_FRAMEBUFFER, mId);
			#ifdef ANDROID
			GLenum target = GL_TEXTURE_2D;
			#else // !ANDROID
			GLenum target = _color->multiSample() ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
			#endif // !ANDROID

			mColor = _color;
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, _color->glId(), 0);
			}

			void setTargetDepth(Texture* _depth) {
			glBindFramebuffer(GL_FRAMEBUFFER, mId);
			#ifdef ANDROID
			GLenum target = GL_TEXTURE_2D;
			#else // !ANDROID
			GLenum target = _depth->multiSample() ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
			#endif // !ANDROID

			mDepth = _depth;
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, target, _depth->glId(), 0);
			}

			unsigned glId() const { return mId; }

			Texture* color() const { return mColor; }
			Texture* depth() const { return mDepth; }

			float aspectRatio() const { return (float(mColor->size().y)) / mColor->size().x; }

			private:
			Texture* mColor = nullptr;
			Texture* mDepth = nullptr;

			// OpenGL specific
			unsigned mId;*/
		};
	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_TYPES_FRAMEBUFFERRT_H_
