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
#include <core/platform/osHandler.h>

using namespace rev::math;

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
			mWinapiHandle = CreateWindowW(L"RevWindowClass",	// Class name, registered by the video driver
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

			// Register for system messages
			// Register in osHandler for message processing
			(*core::OSHandler::get()) += [this](MSG _msg){
				return processWin32Message(_msg);
			};
		}

		//--------------------------------------------------------------------------------------------------------------
		bool WindowWindows::processWin32Message(MSG _msg) {
			if(_msg.message == WM_SIZE)
			{
				LPARAM lparam = _msg.lParam;
				mSize = Vec2u(LOWORD(lparam), HIWORD(lparam));
				invokeOnResize();
				return true;
			}
			if(_msg.message == WM_SIZING)
			{
				RECT* rect = reinterpret_cast<RECT*>(_msg.lParam);
				mSize = Vec2u(rect->right-rect->left, rect->bottom-rect->top);
				invokeOnResize();
				return true;
			}
			return false;
		}

		//--------------------------------------------------------------------------------------------------------------
		LRESULT CALLBACK WindowProc(HWND _hwnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam) {
			MSG msg;
			msg.hwnd = _hwnd;
			msg.lParam = _lParam;
			msg.wParam = _wParam;
			msg.message = _uMsg;
			if(core::OSHandler::get()->processMessage(msg))
				return 0;
			return DefWindowProc(_hwnd, _uMsg, _wParam, _lParam);
		}

		//--------------------------------------------------------------------------------------------------------------
		void WindowWindows::registerClass() {
			HINSTANCE moduleHandle = GetModuleHandle(NULL);
			// -- Register a new window class --
			WNDCLASSW winClass = {
				CS_OWNDC, // Class style
				WindowProc,
				0,
				0,
				moduleHandle,
				NULL,	// Default icon
				NULL,	// No cursor shape
				NULL,
				NULL,
				L"RevWindowClass" };

			RegisterClassW(&winClass);
		}

	}	// namespace video
}	// namespace rev

#endif // _WIN32