//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014-10-29
//----------------------------------------------------------------------------------------------------------------------
// Interface with OpenGL for linux platforms
#include "openGLDriverLinux.h"
#include <video/window/window.h>

#include <GL/glew.h>

namespace rev { 
	namespace video
	{
		OpenGLDriverLinux::OpenGLDriverLinux(const Window* _window) {
			mWindow = _window;
			//GLXContext glc = glXCreateContext(_window->display(), _window->visualInfo(), NULL, GL_TRUE);
			//glXMakeCurrent(_window->display(), _window->xWindow(), glc);
		}

		void OpenGLDriverLinux::swapBuffers() {
			glXSwapBuffers(mWindow->display(), mWindow->xWindow());
		}
		
	} // namespace video
}	// namespace rev