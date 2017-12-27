//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#ifdef ANDROID

#include "../openGL.h"

namespace rev { namespace video {

	class GraphicsDriverGLAndroid
	{
	public:
		static GraphicsDriverGLAndroid* createDriver(ANativeWindow* _nativeWindow);
		void swapBuffers();

	private:
		ANativeWindow*	mWindowHandle;
	};

	typedef GraphicsDriverGLAndroid GraphicsDriverGL;

}} // namespace rev::video

#endif // ANDROID