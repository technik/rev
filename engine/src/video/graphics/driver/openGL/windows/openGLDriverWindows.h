//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/07
//----------------------------------------------------------------------------------------------------------------------
// Interface with OpenGL for Windows platforms

#ifndef _REV_VIDEO_GRAPHICS_DRIVER_OPENGL_WINDOWS_OPENGLDRIVERWINDOWS_H_
#define _REV_VIDEO_GRAPHICS_DRIVER_OPENGL_WINDOWS_OPENGLDRIVERWINDOWS_H_

#ifdef _WIN32
#include <Windows.h>

#include "../../graphicsDriver.h"

namespace rev {
	namespace video {

		class Window;

		class OpenGLDriverWindows : public GraphicsDriver {
		public:
			OpenGLDriverWindows(const Window*);
			void	swapBuffers();

		private:
			HWND	mWindowHandle;
			HDC		mDevCtxHandle;
		};
		
		typedef OpenGLDriverWindows OpenGLDriverBase;

	}	// namespace video
}	// namespace rev

#endif // _WIN32

#endif // _REV_VIDEO_GRAPHICS_DRIVER_OPENGL_WINDOWS_OPENGLDRIVERWINDOWS_H_