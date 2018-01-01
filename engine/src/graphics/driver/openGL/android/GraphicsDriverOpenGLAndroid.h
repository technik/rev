//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#ifdef ANDROID

#include "../openGL.h"

namespace rev { namespace graphics {

	class GraphicsDriverGLAndroid
	{
	public:
		using NativeWindow = ANativeWindow*;

		static GraphicsDriverGLAndroid* createDriver(NativeWindow _nativeWindow);
		void swapBuffers();

	private:
		ANativeWindow*	mWindowHandle;
	};

	typedef GraphicsDriverGLAndroid GraphicsDriverGL;

}} // namespace rev::graphics

#endif // ANDROID