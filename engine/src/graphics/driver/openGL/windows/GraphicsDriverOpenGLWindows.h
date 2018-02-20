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

		//
	};

	class GraphicsDriverGL;

	class GraphicsDriverGLWindows
	{
	public:
		using NativeWindow = WindowWin32*;

		static GraphicsDriverGL* createDriver(NativeWindow _nativeWindow);
		NativeWindow nativeWindow() const { return window; }
		void swapBuffers();

	private:
		NativeWindow	window;
		HWND	mWindowHandle;
		HDC		mDevCtxHandle;
	};

	typedef GraphicsDriverGLWindows GraphicsDriverGLBase;

}} // namespace rev::graphics

#endif // _WIN32