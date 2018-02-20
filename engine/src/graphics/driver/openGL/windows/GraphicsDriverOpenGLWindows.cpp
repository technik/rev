//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#ifdef _WIN32

#include "GraphicsDriverOpenGLWindows.h"
#include "../GraphicsDriverOpenGL.h"
#include <iostream>

using namespace std;

namespace rev {	namespace graphics {

	//------------------------------------------------------------------------------------------------------------------
	GraphicsDriverGL* GraphicsDriverGLWindows::createDriver(NativeWindow _window) {
		auto deviceContext = GetDC(_window->nativeWindow); // Device contex
												  // Set pixel format
		PIXELFORMATDESCRIPTOR pfd = {
			sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
			1,											// Version Number
			PFD_DRAW_TO_WINDOW |						// Format Must Support Window
			PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
			PFD_SWAP_EXCHANGE |							// Prefer framebuffer swap
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

		GLuint pixelFormat = ChoosePixelFormat(deviceContext, &pfd);
		DescribePixelFormat(deviceContext, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
		std::cout << "Retrieved pixel format:\n"
			<< "Color depth: " << (size_t)pfd.cColorBits << "\n"
			<< "Z-Buffer depth: " << (size_t)pfd.cDepthBits << "\n"
			<< "Stencil depth: " << (size_t)pfd.cStencilBits << "\n";
		SetPixelFormat(deviceContext, pixelFormat, &pfd);

		// Create a dummy context to retrieve glew extensions, so we can then create a proper context
		HGLRC dummyCtxHandle = wglCreateContext(deviceContext);
		wglMakeCurrent(deviceContext, dummyCtxHandle);
		// Load extensions
		GLenum res = glewInit();
		if (res != GLEW_OK) {
			cout << "Error: " << glewGetErrorString(res) << "\n";
			return nullptr;
		}
		// Try to create context with attributes
		if (wglCreateContextAttribsARB) { // Advanced context creation is available
										  // Destroy dummy context ...
										  // ... and create a new one
			int contextAttribs[] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
				WGL_CONTEXT_MINOR_VERSION_ARB, 5,
				WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
				NULL
			};
			HGLRC renderContext = wglCreateContextAttribsARB(deviceContext, NULL, contextAttribs);
			if (renderContext) {
				wglMakeCurrent(deviceContext, renderContext);
				wglDeleteContext(dummyCtxHandle);
			}
			else
				cout << "Unable to create render context with attributes. Using default context\n";
		}
		cout << "OpenGL Version " << (char*)glGetString(GL_VERSION) << "\n";
		cout << "GLSL Version " << (char*)glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";

		auto driver = new GraphicsDriverGL(std::make_unique<DefaultFrameBuffer>(_window->size));
		if(driver) {
			driver->window = _window;
			driver->mWindowHandle = _window->nativeWindow;
			driver->mDevCtxHandle = deviceContext;
		}
		return driver;
	}

	//------------------------------------------------------------------------------------------------------------------
	void GraphicsDriverGLWindows::swapBuffers() {
		SwapBuffers(mDevCtxHandle);
	}

} }	// namespace rev::graphics

#endif // _WIN32