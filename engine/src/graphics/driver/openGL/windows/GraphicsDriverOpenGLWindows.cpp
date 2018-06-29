//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#ifdef _WIN32

#include "GraphicsDriverOpenGLWindows.h"
#include "../GraphicsDriverOpenGL.h"
#include <iostream>
#include <core/platform/osHandler.h>

using namespace std;

namespace rev {	namespace graphics {

	GraphicsDriverGL* GraphicsDriverGL::s_instance = nullptr;

	void APIENTRY gfxDebugCallback(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar *message,
		const void *userParam)
	{
		cout << message << "\n";
	}

	namespace {
		static bool sIsWindowClassRegistered = false;

		//--------------------------------------------------------------------------------------------------------------
		LRESULT CALLBACK WindowProc(HWND _hwnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam) {
			MSG msg;
			msg.hwnd = _hwnd;
			msg.lParam = _lParam;
			msg.wParam = _wParam;
			msg.message = _uMsg;
			auto osMessageProcessor = core::OSHandler::get();
			if(osMessageProcessor)
				if(osMessageProcessor->processMessage(msg))
					return 0;
			return DefWindowProc(_hwnd, _uMsg, _wParam, _lParam);
		}

		//--------------------------------------------------------------------------------------------------------------
		void registerClass() {
			HINSTANCE moduleHandle = GetModuleHandle(NULL);
			// -- Register a new window class --
			WNDCLASS winClass = {
				CS_OWNDC, // Class style
				WindowProc,
				0,
				0,
				moduleHandle,
				NULL,	// Default icon
				NULL,	// No cursor shape
				NULL,
				NULL,
				"RevWindowClass" };

			RegisterClass(&winClass);
		}

		//--------------------------------------------------------------------------------------------------------------
		WindowWin32 createDummyWindow()
		{
			if (!sIsWindowClassRegistered)
				registerClass();

			// Create a windown through the windows API
			HWND mWinapiHandle = CreateWindow("RevWindowClass",	// Class name, registered by the video driver
				"",
				WS_DISABLED, // Prevent user interaction
				0,						// X Position
				0,						// Y Position
				0,				// Width
				0,				// Height
				0, 0, 0, 0);				// Windows specific parameters that we don't need

			rev::graphics::WindowWin32 window;
			window.nativeWindow = mWinapiHandle;
			return window;
		}

	}	// anonymous namespace

	//--------------------------------------------------------------------------------------------------------------
	WindowWin32 WindowWin32::createWindow(const math::Vec2u& _pos, const math::Vec2u& _size, const char* _windowName, bool _visible) {
		if (!sIsWindowClassRegistered)
			registerClass();

		// Create a windown through the windows API
		HWND mWinapiHandle = CreateWindow("RevWindowClass",	// Class name, registered by the video driver
			_windowName,
			WS_SIZEBOX | WS_CAPTION | WS_POPUP | (_visible? WS_VISIBLE : WS_DISABLED),	// Creation options
			_pos.x(),						// X Position
			_pos.y(),						// Y Position
			int(_size.x()),				// Width
			int(_size.y()),				// Height
			0, 0, 0, 0);				// Windows specific parameters that we don't need

										// Resize client area
		RECT rcClient;
		POINT ptDiff;
		GetClientRect(mWinapiHandle, &rcClient);
		ptDiff.x = _size.x() - rcClient.right;
		ptDiff.y = _size.y() - rcClient.bottom;
		MoveWindow(mWinapiHandle, _pos.x(), _pos.y(), _size.x() + ptDiff.x, _size.y() + ptDiff.y, TRUE);
		// Note: Maybe we could do this using SM_CYCAPTION and SM_CYBORDER instead of resizing a window.

		rev::graphics::WindowWin32 window;
		window.nativeWindow = mWinapiHandle;
		window.size = _size;
		return window;
	}

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
	bool loadOpenGLExtensions(HWND& dummyWindowHandle, HGLRC& dummyRenderContext)
	{
		// Create a dummy window
		auto dummyWindow = createDummyWindow();
		dummyWindowHandle = dummyWindow.nativeWindow;
		// Set dummy pixel format
		auto deviceContext = GetDC(dummyWindowHandle);
		setDummyPixelFormat(deviceContext);
		// Create dummy context
		dummyRenderContext = wglCreateContext(deviceContext);
		wglMakeCurrent(deviceContext, dummyRenderContext);
		
		// Load extensions
		GLenum res = glewInit();
		if (res != GLEW_OK) {
			cout << "Error: " << glewGetErrorString(res) << "\n";
			return false;
		}

		return true;
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
//			intPFAttributes.push_back(WGL_COLORSPACE_EXT);
//			intPFAttributes.push_back(WGL_COLORSPACE_SRGB_EXT);
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
			WGL_CONTEXT_MINOR_VERSION_ARB, 2,
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

	//------------------------------------------------------------------------------------------------------------------
	GraphicsDriverGL* GraphicsDriverGLWindows::createDriver(NativeWindow _window) {
		HGLRC dummyRenderContext;
		HWND dummyWindow;
		if(!loadOpenGLExtensions(dummyWindow, dummyRenderContext))
			return nullptr;

		bool sRGBFrameBuffer = false, sRGBTextures = false;
		checkExtensions(sRGBTextures, sRGBFrameBuffer);

		// Try to create context with attributes
		auto deviceContext = GetDC(_window->nativeWindow); // Device contex
		auto contexCreatedOk = createFinalOpenGLContext(deviceContext, sRGBFrameBuffer);

		// Destroy temporary context and window
		wglDeleteContext(dummyRenderContext);
		DestroyWindow(dummyWindow);

		if(contexCreatedOk)
		{
			// Proper driver initialization
			cout << "OpenGL Version " << (char*)glGetString(GL_VERSION) << "\n";
			cout << "GLSL Version " << (char*)glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
			auto driver = new GraphicsDriverGL(std::make_unique<DefaultFrameBuffer>(_window->size));
			if(driver) {
				driver->window = _window;
				driver->mSupportSRGBTextures = sRGBTextures;
				driver->mSupportSRGBFrameBuffer = sRGBFrameBuffer;
				driver->mWindowHandle = _window->nativeWindow;
				driver->mDevCtxHandle = deviceContext;
#ifndef NDEBUG
                glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
				glDebugMessageCallback(gfxDebugCallback, nullptr);
#endif
			}
			return driver;
		}
		else
		{
			core::Log::error("failed OpenGL context creation with attributes");
			return nullptr;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void GraphicsDriverGLWindows::swapBuffers() {
		SwapBuffers(mDevCtxHandle);
	}

	//------------------------------------------------------------------------------------------------------------------
	void GraphicsDriverGLWindows::checkExtensions(bool& sRGBTextures, bool& sRGBFrameBuffer) 
	{
		GLint nExtensions;
		glGetIntegerv(GL_NUM_EXTENSIONS, &nExtensions);
		cout << "OpenGL extensions supported: (" << nExtensions << ")\n";
		for(GLint i = 0; i < nExtensions; ++i)
		{
			auto extensionName = glGetStringi(GL_EXTENSIONS, i);
			if(string("GL_EXT_texture_sRGB") == (char*)extensionName)
				sRGBTextures = true;
			if(string("GL_ARB_framebuffer_sRGB") == (char*)extensionName)
				sRGBFrameBuffer = true;

			core::Log::verbose(extensionName);
		}
	}

} }	// namespace rev::graphics

#endif // _WIN32