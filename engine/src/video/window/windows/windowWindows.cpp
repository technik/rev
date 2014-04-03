//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Window for Win32 systems
#ifdef _WIN32

#include <cassert>
#include <cstdlib>

#include <Windows.h>

#include "windowWindows.h"

namespace rev {
	namespace video {

		const size_t MAX_NAME_SIZE = 1024;

		//--------------------------------------------------------------------------------------------------------------
		bool WindowWindows::sIsClassRegistered = false;

		//--------------------------------------------------------------------------------------------------------------
		WindowWindows::WindowWindows(const math::Vec2u& _pos, const math::Vec2u& _size, const char* _windowName) {
			if (!sIsClassRegistered)
				registerClass();

			// Assign internal values
			mPosition = _pos;
			mSize = _size;

			// Convert window name to utf16 for windows
			size_t nameSize = strlen(_windowName);
			assert(nameSize < MAX_NAME_SIZE);
			wchar_t utf16Name[MAX_NAME_SIZE];
			mbstowcs_s(&nameSize, utf16Name, _windowName, MAX_NAME_SIZE);

			// Create a windown through the windows API
			mWinapiHandle = CreateWindow(L"RevWindowClass",	// Class name, registered by the video driver
				utf16Name,								// Window name (currently unsupported
				WS_SIZEBOX | WS_CAPTION | WS_POPUP | WS_VISIBLE,	// Creation options
				_pos.x,						// X Position
				_pos.y,						// Y Position
				int(_size.x),				// Width
				int(_size.y),				// Height
				0, 0, 0, 0);				// Windows specific parameters that we don't need

			// Resize client area
			RECT rcClient;
			POINT ptDiff;
			GetClientRect(mWinapiHandle, &rcClient);
			ptDiff.x = mSize.x - rcClient.right;
			ptDiff.y = mSize.y - rcClient.bottom;
			MoveWindow(mWinapiHandle, mPosition.x, mPosition.y, mSize.x + ptDiff.x, mSize.y + ptDiff.y, TRUE);

			// Note: Maybe we could do this using SM_CYCAPTION and SM_CYBORDER instead of resizing a window.
		}

		//--------------------------------------------------------------------------------------------------------------
		void WindowWindows::registerClass() {
			HINSTANCE moduleHandle = GetModuleHandle(NULL);
			// -- Register a new window class --
			WNDCLASS winClass = {
				CS_OWNDC, // Class style
				DefWindowProc,
				0,
				0,
				moduleHandle,
				NULL,	// Default icon
				NULL,	// No cursor shape
				NULL,
				NULL,
				L"RevWindowClass" };

			RegisterClass(&winClass);
		}

	}	// namespace video
}	// namespace rev

#endif // _WIN32