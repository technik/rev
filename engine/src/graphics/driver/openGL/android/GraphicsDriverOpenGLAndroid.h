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

	class GraphicsDriverGL;

	class GraphicsDriverGLAndroid
	{
	public:
		using NativeWindow = AndroidScreen*;

		static GraphicsDriverGL* createDriver(AndroidScreen* _screen);
		math::Vec2i windowSize() const { return mSize; }
		void swapBuffers();

	private:
		AndroidScreen*	mWindow = nullptr;

		math::Vec2i mSize;
		EGLSurface surface;
		EGLContext context;
		EGLDisplay display;
	};

	typedef GraphicsDriverGLAndroid GraphicsDriverGLBase;

}} // namespace rev::graphics

#endif // ANDROID