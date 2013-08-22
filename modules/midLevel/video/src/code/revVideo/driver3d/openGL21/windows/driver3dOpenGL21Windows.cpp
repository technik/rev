//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 11th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Windows openGL 2.1 driver

#ifdef WIN32

#include <Windows.h>
#include <gl/GL.h>

#include "driver3dOpenGL21Windows.h"
#include <revVideo/types/window/windows/videoWindowWindows.h>

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	Driver3dOpenGL21Windows::Driver3dOpenGL21Windows(const Window * _window)
	{
		const WindowWindows * wnd = static_cast<const WindowWindows*>(_window);
		mWindowHandle = wnd->winapiHandle();

		static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
		{
			sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
			1,											// Version Number
			PFD_DRAW_TO_WINDOW |						// Format Must Support Window
			PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
			PFD_DOUBLEBUFFER,							// Must Support Double Buffering
			PFD_TYPE_RGBA,								// Request An RGBA Format
			16,											// Color Depth
			0, 0, 0, 0, 0, 0,							// Color Bits Ignored
			0,											// No Alpha Buffer
			0,											// Shift Bit Ignored
			0,											// No Accumulation Buffer
			0, 0, 0, 0,									// Accumulation Bits Ignored
			24,											// 16Bit Z-Buffer (Depth Buffer)  
			0,											// No Stencil Buffer
			0,											// No Auxiliary Buffer
			PFD_MAIN_PLANE,								// Main Drawing Layer
			0,											// Reserved
			0, 0, 0										// Layer Masks Ignored
		};
	
		// -- Some Windows-OpenGL configuration --
		mDrawCtxHandle = GetDC(mWindowHandle); // Drawing context
		GLuint pixelFormat = ChoosePixelFormat(mDrawCtxHandle,&pfd);
		SetPixelFormat(mDrawCtxHandle,pixelFormat,&pfd);
		HGLRC renderCtxHandle=wglCreateContext(mDrawCtxHandle);
		wglMakeCurrent(mDrawCtxHandle,renderCtxHandle);
	
		// Note extensions can only be loaded after OpenGL context has been created
		loadOpenGLExtensions();
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21Windows::swapBuffers()
	{
		SwapBuffers(mDrawCtxHandle);
	}

}	// namespace video
}	// namespace rev

#endif // WIN32
