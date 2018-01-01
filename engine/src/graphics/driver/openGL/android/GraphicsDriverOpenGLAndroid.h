//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#ifdef ANDROID

#include "../openGL.h"
#include <math/algebra/vector.h>

namespace rev { namespace graphics {

	class GraphicsDriverGLAndroid
	{
	public:
		using NativeWindow = ANativeWindow*;

		static GraphicsDriverGLAndroid* createDriver(NativeWindow _nativeWindow);
		void swapBuffers();

	private:
		ANativeWindow*	mWindowHandle = nullptr;

		math::Vec2i mSize;
		EGLSurface surface;
		EGLContext context;
		EGLDisplay display;
	};

	typedef GraphicsDriverGLAndroid GraphicsDriverGL;

}} // namespace rev::graphics

#endif // ANDROID