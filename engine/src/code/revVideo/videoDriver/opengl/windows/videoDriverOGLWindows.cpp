////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, linux video driver
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 21st, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Windows openGL video Driver
#ifdef WIN32

// Standard headers
#include <windows.h>
#include <gl/GL.h>

// engine headers
#include "videoDriverOGLWindows.h"

// Global data
 HWND	g_hWnd = 0;
 HDC	g_hDC = 0;

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CVideoDriverOGLWindows::CVideoDriverOGLWindows()
	{
		createOpenGLWindow(int(screenSize().x), int(screenSize().y));
	}

	//------------------------------------------------------------------------------------------------------------------
	CVideoDriverOGLWindows::~CVideoDriverOGLWindows()
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoDriverOGLWindows::endFrame()
	{
		IVideoDriverOpenGL::endFrame();
		SwapBuffers(g_hDC);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoDriverOGLWindows::createOpenGLWindow(const unsigned int _width, const unsigned int _height)
	{
	HINSTANCE       hInstance = GetModuleHandle(NULL);

	// -- Register a new window class --
	WNDCLASSA winClass = {
		CS_HREDRAW | CS_VREDRAW | CS_OWNDC, // Class style
		DefWindowProc,
		0,
		0,
		hInstance,
		NULL,	// Default icon
		LoadCursor(NULL, IDC_ARROW),	// No cursor shape
		NULL,
		NULL,
		"RevolutionWndClass"};
	RegisterClassA(&winClass);

	CVec2 screenPosition = screenPos();
	// -- Actual creation of the window --
	g_hWnd = CreateWindowA("RevolutionWndClass", "",// Class and name
		WS_POPUP | WS_VISIBLE,						// Creation options
		int(screenPosition.x),
		int(screenPosition.y),						// Position
		_width, _height,							// Size
		0, 0, 0, 0);								// Windows specific parameters that we don't need

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
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	// -- Some Windows-OpenGL configuration --
	g_hDC= GetDC(g_hWnd); // Drawing context
	GLuint PixelFormat=ChoosePixelFormat(g_hDC,&pfd);
	SetPixelFormat(g_hDC,PixelFormat,&pfd);
	HGLRC hRC=wglCreateContext(g_hDC);
	wglMakeCurrent(g_hDC,hRC);
	}
}	// namespace video
}	// namespace rev

#endif // WIN32
