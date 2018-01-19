//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#ifdef _WIN32

#include "../openGL.h"
#include <math/algebra/vector.h>

namespace rev { namespace graphics {

	class WindowWin32
	{
	public:
		HWND nativeWindow;
		math::Vec2u size;
	};

	class GraphicsDriverGLWindows
	{
	public:
		using NativeWindow = WindowWin32*;

		static GraphicsDriverGLWindows* createDriver(NativeWindow _nativeWindow);
		void swapBuffers();

	private:
		HWND	mWindowHandle;
		HDC		mDevCtxHandle;
	};

	typedef GraphicsDriverGLWindows GraphicsDriverGL;

}} // namespace rev::graphics

#endif // _WIN32