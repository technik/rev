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

namespace rev :: gfx
{
	namespace {
		static bool sIsWindowClassRegistered = false;

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
		void registerClass() {
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

			RegisterClass(&winClass);
		}
	}
	//----------------------------------------------------------------------------------------------
	HWND createDummyWindow()
	{
		if (!sIsWindowClassRegistered)
			registerClass();

		// Create a windown through the windows API
		return CreateWindow("RevWindowClass",	// Class name, registered by the video driver
			"",
			WS_DISABLED, // Prevent user interaction
			0,						// X Position
			0,						// Y Position
			0,				// Width
			0,				// Height
			0, 0, 0, 0);				// Windows specific parameters that we don't need
	}
}

#endif // _WIN32