//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/07
//----------------------------------------------------------------------------------------------------------------------
// Interface with OpenGL for linux platforms

#ifndef _REV_VIDEO_GRAPHICS_DRIVER_OPENGL_LINUX_OPENGLDRIVERLINUX_H_
#define _REV_VIDEO_GRAPHICS_DRIVER_OPENGL_LINUX_OPENGLDRIVERLINUX_H_

#ifdef __linux__

#include "../../graphicsDriver.h"

namespace rev {
	namespace video {

		class Window;

		class OpenGLDriverLinux : public GraphicsDriver {
		public:
			OpenGLDriverLinux(const Window*);
			void	swapBuffers() {}

		private:
		};
		
		typedef OpenGLDriverLinux OpenGLDriverBase;

	}	// namespace video
}	// namespace rev

#endif // __linux__

#endif // _REV_VIDEO_GRAPHICS_DRIVER_OPENGL_LINUX_OPENGLDRIVERLINUX_H_