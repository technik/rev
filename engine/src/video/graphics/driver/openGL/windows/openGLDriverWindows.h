//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/07
//----------------------------------------------------------------------------------------------------------------------
// Interface with OpenGL for Windows platforms
#pragma once

#ifdef _WIN32
#include <Windows.h>

#include "../openGLDriver.h"

namespace rev {
	namespace video {

		class Window;

		class OpenGLDriverWindows  {
		public:
			OpenGLDriverWindows(Window*);

			void	swapBuffers();

		private:
			HWND	mWindowHandle;
			HDC		mDevCtxHandle;
		};

	}	// namespace video
}	// namespace rev

#endif // _WIN32