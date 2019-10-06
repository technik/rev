//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2018 Carmelo J Fdez-Aguera
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#pragma once

#include "../OpenGL/openGL.h"
#include "../Windows/windowsPlatform.h"
#include "deviceOpenGLWindows.h"
#include "renderQueueOpenGLWindows.h"

#include <iostream>
#include <string>
#include <vector>

namespace rev :: gfx
{
	//------------------------------------------------------------------------------------------------------------------
	void setDummyPixelFormat(HDC deviceContext)
	{
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
			8,											// Stencil Buffer
			0,											// No Auxiliary Buffer
			PFD_MAIN_PLANE,								// Main Drawing Layer
			0,											// Reserved
			0, 0, 0										// Layer Masks Ignored
		};

		GLuint pixelFormat = ChoosePixelFormat(deviceContext, &pfd);
		DescribePixelFormat(deviceContext, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
		SetPixelFormat(deviceContext, pixelFormat, &pfd);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool setFinalPixelFormat(HDC deviceContext, bool sRGBFrameBuffer)
	{
		std::vector<int> intPFAttributes = {
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB, 32,
			WGL_DEPTH_BITS_ARB, 24,
		};
		if(sRGBFrameBuffer)
		{
			intPFAttributes.push_back(WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB);
			intPFAttributes.push_back(GL_TRUE);
			intPFAttributes.push_back(WGL_COLORSPACE_EXT);
			intPFAttributes.push_back(WGL_COLORSPACE_SRGB_EXT);
		};
		intPFAttributes.push_back(0);

		int pixelFormat;
		UINT numFormats;

		if(wglChoosePixelFormatARB(
			deviceContext,
			intPFAttributes.data(),
			nullptr, 
			1,
			&pixelFormat,
			&numFormats))
		{
			PIXELFORMATDESCRIPTOR pfd;
			DescribePixelFormat(deviceContext, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
			SetPixelFormat(deviceContext, pixelFormat, &pfd);
			return true;
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool createFinalOpenGLContext(HDC deviceContext, bool sRGBFrameBuffer)
	{
		if (!wglCreateContextAttribsARB) 
			return false;

		// Set proper pixel format
		if(!setFinalPixelFormat(deviceContext, sRGBFrameBuffer))
			return false;

		// Set context attributes
		int contextAttribs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			WGL_CONTEXT_MINOR_VERSION_ARB, 5,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			WGL_CONTEXT_FLAGS_ARB, 0
#ifdef _DEBUG
			| WGL_CONTEXT_DEBUG_BIT_ARB
#endif
			,NULL
		};
		// Create context
		HGLRC renderContext = wglCreateContextAttribsARB(deviceContext, NULL, contextAttribs);
		if (renderContext) {
			wglMakeCurrent(deviceContext, renderContext);
			return true;
		}

		return false;
	}
	
	void APIENTRY gfxDebugCallback(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar *message,
		const void *userParam)
	{
		std::cout << message << "\n";
	}

	//----------------------------------------------------------------------------------------------
	DeviceOpenGLWindows::DeviceOpenGLWindows(HWND targetWindow, bool sRGBFrameBuffer)
	{
		// Windows specific initialization
		// create an invisible window to init the context
		m_windowHandle = createDummyWindow();

		// Common initialization
		// Set dummy pixel format
		auto deviceContext = GetDC(m_windowHandle);
		setDummyPixelFormat(deviceContext);
		// Create dummy context
		auto dummyRenderContext = wglCreateContext(deviceContext);
		wglMakeCurrent(deviceContext, dummyRenderContext);

		// Load extensions
		GLenum res = glewInit();
		if (res != GLEW_OK) {
			std::cout << "Error: " << glewGetErrorString(res) << "\n";

			DestroyWindow(m_windowHandle);
			m_windowHandle = 0;
			return;
		}

		// If no target window was provided, create an invisible one
		if(!targetWindow)
			targetWindow = createWindow({0,0}, {0,0}, "", false, false);

		//checkSupportedExtensions();

		// Try to create context with attributes
		m_deviceContext = GetDC(targetWindow); // Device contex
		auto contexCreatedOk = createFinalOpenGLContext(m_deviceContext, (sRGBFrameBuffer && m_supportSRGBFrameBuffer));

		// Destroy temporary context and window
		wglDeleteContext(dummyRenderContext);
		DestroyWindow(m_windowHandle); // Destroy dummy window

		if(contexCreatedOk)
		{
			m_windowHandle = targetWindow;
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
			glDebugMessageCallback(gfxDebugCallback, nullptr);

			readDeviceLimits();

			m_renderQueue = new RenderQueueOpenGLWindows(m_deviceContext, *this);
		}
	}

	//----------------------------------------------------------------------------------------------
	DeviceOpenGLWindows::~DeviceOpenGLWindows()
	{
		if(m_windowHandle)
		{
			DestroyWindow(m_windowHandle);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	FrameBuffer DeviceOpenGLWindows::defaultFrameBuffer()
	{
		return FrameBuffer(0);
	}

	//------------------------------------------------------------------------------------------------------------------
	void DeviceOpenGLWindows::checkSupportedExtensions() 
	{
		GLint nExtensions;
		glGetIntegerv(GL_NUM_EXTENSIONS, &nExtensions);
		for(GLint i = 0; i < nExtensions; ++i)
		{
			auto extensionName = glGetStringi(GL_EXTENSIONS, i);
			if(std::string("GL_EXT_texture_sRGB") == (char*)extensionName)
				m_supportSRGBTextures = true;
			if(std::string("GL_ARB_framebuffer_sRGB") == (char*)extensionName)
				m_supportSRGBFrameBuffer = true;
		}
	}
}
