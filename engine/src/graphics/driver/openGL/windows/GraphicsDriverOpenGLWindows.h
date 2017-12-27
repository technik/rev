//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#ifdef _WIN32

#include "../openGL.h"
#include <video/window/window.h>

namespace rev { namespace video {

	class GraphicsDriverGLWindows
	{
	public:
		static GraphicsDriverGLWindows* createDriver(Window* _nativeWindow);
		void swapBuffers();

	private:
		HWND	mWindowHandle;
		HDC		mDevCtxHandle;
	};

	typedef GraphicsDriverGLWindows GraphicsDriverGL;

}} // namespace rev::video

#endif // _WIN32