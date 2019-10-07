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
#ifdef _WIN32

#include "windowsPlatform.h"
#include <core/platform/osHandler.h>
#include <iostream>

namespace rev :: gfx
{
	namespace {
		bool sIsWindowClassRegistered = false;

		//------------------------------------------------------------------------------------------
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

		//------------------------------------------------------------------------------------------
		bool registerClass() {
			if(sIsWindowClassRegistered)
				return true;

			HINSTANCE moduleHandle = GetModuleHandle(NULL);
			// -- Register a new window class --
			WNDCLASS winClass = {
				CS_OWNDC | CS_HREDRAW | CS_VREDRAW, // Class style
				WindowProc,
				0,
				0,
				moduleHandle,
				NULL,	// Default icon
				NULL,	// No cursor shape
				NULL,
				NULL,
				"RevWindowClass" };

			if(!RegisterClass(&winClass))
			{
				auto error = GetLastError();
				std::cout << "Failed to register window class. Error code: " << error << "\n";
				return false;
			}
			sIsWindowClassRegistered = true;
			return true;
		}
	}
	//----------------------------------------------------------------------------------------------
	HWND createDummyWindow()
	{
		if(!registerClass())
			return NULL;

		// Create a windown through the windows API
		return CreateWindow("RevWindowClass",	// Class name, registered by the video driver
			"Rev",
			WS_DISABLED, // Prevent user interaction
			0,				// X Position
			0,				// Y Position
			0,				// Width
			0,				// Height
			0, 0, 0, 0);				// Windows specific parameters that we don't need
	}

	//----------------------------------------------------------------------------------------------
	HWND createWindow(const math::Vec2u& _pos, const math::Vec2u& _size, const char* _windowName, bool _visible, bool fullScreen)
	{
		if (!registerClass())
			return NULL;

		math::Vec2u windowSize = _size;
		math::Vec2u windowPos = _pos;
		DWORD windowStyle = WS_POPUP | (_visible ? WS_VISIBLE : WS_DISABLED);
		DWORD windowStyleEx = 0;
		if (fullScreen)
		{
			windowStyleEx = WS_EX_APPWINDOW;
			windowSize.x() = GetSystemMetrics(SM_CXSCREEN);
			windowSize.y() = GetSystemMetrics(SM_CYSCREEN);
			windowPos = math::Vec2u::zero();
		}
		else
		{
			windowStyle |= WS_OVERLAPPEDWINDOW;
		}

		// Create a windown through the windows API
		HWND nativeHandle = CreateWindowEx(
			windowStyleEx,
			"RevWindowClass",	// Class name, registered by the video driver
			_windowName,
			windowStyle,		// Creation options
			windowPos.x(),		// X Position
			windowPos.y(),		// Y Position
			int(windowSize.x()),// Width
			int(windowSize.y()),// Height
			0, 0, 0, 0);		// Windows specific parameters that we don't need

		// Resize client area
		{
			RECT rcClient;
			POINT ptDiff;
			GetClientRect(nativeHandle, &rcClient);
			ptDiff.x = windowSize.x() - rcClient.right;
			ptDiff.y = windowSize.y() - rcClient.bottom;
			MoveWindow(nativeHandle, windowPos.x(), windowPos.y(), windowSize.x() + ptDiff.x, windowSize.y() + ptDiff.y, TRUE);
			// Note: Maybe we could do this using SM_CYCAPTION and SM_CYBORDER instead of resizing a window.
			// Or see: https://www.3dgep.com/learning-directx12-1/
		}

		return nativeHandle;
	}
}

#endif // _WIN32