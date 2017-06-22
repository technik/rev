//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/07
//----------------------------------------------------------------------------------------------------------------------
// Interface with OpenGL for Windows platforms

#ifdef _WIN32

#include "openGLDriverWindows.h"
#include <video/window/window.h>
#include <core/platform/osHandler.h>
#include <core/platform/fileSystem/file.h>
#include <string>
#include <gl/GL.h>
#include <core/platform/fileSystem/fileSystem.h>
#include <iostream>

using namespace std;
using namespace rev::core;

namespace rev {
	namespace video {
		//--------------------------------------------------------------------------------------------------------------
		OpenGLDriverWindows::OpenGLDriverWindows(Window* _window)
		{
			mWindowHandle = _window->winapiHandle();
			mDevCtxHandle = GetDC(mWindowHandle); // Device contex

			// Set pixel format
			static PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
			{
				sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
				1,											// Version Number
				PFD_DRAW_TO_WINDOW |						// Format Must Support Window
				PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
				PFD_DOUBLEBUFFER,							// Must Support Double Buffering
				PFD_TYPE_RGBA,								// Request An RGBA Format
				32,											// Color Depth
				0, 0, 0, 0, 0, 0,							// Color Bits Ignored
				0,											// No Alpha Buffer
				0,											// Shift Bit Ignored
				0,											// No Accumulation Buffer
				0, 0, 0, 0,									// Accumulation Bits Ignored
				24,											// 24 Bit Z-Buffer (Depth Buffer)  
				8,											// No Stencil Buffer
				0,											// No Auxiliary Buffer
				PFD_MAIN_PLANE,								// Main Drawing Layer
				0,											// Reserved
				0, 0, 0										// Layer Masks Ignored
			};

			GLuint pixelFormat = ChoosePixelFormat(mDevCtxHandle, &pfd);
			DescribePixelFormat(mDevCtxHandle, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
			std::cout << "Retrieved pixel format:\n"
				<< "Color depth: " << (size_t)pfd.cColorBits << "\n"
				<< "Z-Buffer depth: " << (size_t)pfd.cDepthBits << "\n"
				<< "Stencil depth: " << (size_t)pfd.cStencilBits << "\n";
			SetPixelFormat(mDevCtxHandle, pixelFormat, &pfd);

			// Set context
			HGLRC renderCtxHandle = wglCreateContext(mDevCtxHandle);
			wglMakeCurrent(mDevCtxHandle, renderCtxHandle);

			OpenGLDriver::init(_window);
		}

		//--------------------------------------------------------------------------------------------------------------
		void OpenGLDriverWindows::swapBuffers() {
			SwapBuffers(mDevCtxHandle);
		}

	}	// namespace video
}	// namespace rev

#endif // _WIN32