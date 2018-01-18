//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#ifdef ANDROID

#include "../openGL.h"
#include <math/algebra/vector.h>

namespace rev { namespace graphics {

	class AndroidScreen
	{
	public:
		ANativeWindow* nativeWindow = nullptr;
		math::Vec2u size;
	};

	class GraphicsDriverGLAndroid
	{
	public:
		using NativeWindow = AndroidScreen*;

		static GraphicsDriverGLAndroid* createDriver(AndroidScreen* _screen);
		void swapBuffers();

	private:
		AndroidScreen*	mWindow = nullptr;

		math::Vec2i mSize;
		EGLSurface surface;
		EGLContext context;
		EGLDisplay display;
	};

	typedef GraphicsDriverGLAndroid GraphicsDriverGL;

}} // namespace rev::graphics

#endif // ANDROID