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
		OpenGLDriverWindows::OpenGLDriverWindows(Window* _window, const cjson::Json& _config)
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

			// Create a dummy context to retrieve glew extensions, so we can then create a proper context
			HGLRC dummyCtxHandle = wglCreateContext(mDevCtxHandle);
			wglMakeCurrent(mDevCtxHandle, dummyCtxHandle);
			// Load extensions
#ifdef _DEBUG
			queryExtensions();
#endif // _DEBUG
			GLenum res = glewInit();
			if (res != GLEW_OK) {
				cout << "Error: " << glewGetErrorString(res) << "\n";
				assert(false);
			}
			// Try to create context with attributes
			if (wglCreateContextAttribsARB) { // Advanced context creation is available
				// Destroy dummy context ...
				// ... and create a new one
				int contextAttribs[] = {
					WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
					WGL_CONTEXT_MINOR_VERSION_ARB, 0,
					WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
					NULL
				};
				HGLRC renderContext = wglCreateContextAttribsARB(mDevCtxHandle, NULL, contextAttribs);
				if (renderContext) {
					wglMakeCurrent(mDevCtxHandle, renderContext);
					wglDeleteContext(dummyCtxHandle);
				}
				else
					cout << "Unable to create render context with attributes. Using default context\n";
			}
			cout << "OpenGL Version " << (char*)glGetString(GL_VERSION) << "\n";
			cout << "GLSL Version " << (char*)glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";

			OpenGLDriver::init(_window);
		}

		//--------------------------------------------------------------------------------------------------------------
		void OpenGLDriverWindows::swapBuffers() {
			SwapBuffers(mDevCtxHandle);
		}

	}	// namespace video
}	// namespace rev

#endif // _WIN32