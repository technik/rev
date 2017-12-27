//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#ifdef _WIN32

#include "../openGL.h"

namespace rev { namespace graphics {

	class GraphicsDriverGLWindows
	{
	public:
		typedef HWND NativeWindow;

		static GraphicsDriverGLWindows* createDriver(NativeWindow _nativeWindow);
		void swapBuffers();

	private:
		HWND	mWindowHandle;
		HDC		mDevCtxHandle;
	};

	typedef GraphicsDriverGLWindows GraphicsDriverGL;

}} // namespace rev::graphics

#endif // _WIN32