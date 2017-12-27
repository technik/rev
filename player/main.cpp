//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
// Engine's default player's entry point
#ifdef _WIN32
#include "player.h"

#include <Windows.h>
#include <math/algebra/vector.h>

using namespace rev::math;

//--------------------------------------------------------------------------------------------------------------
static bool sIsWindowClassRegistered = false;

//--------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowProc(HWND _hwnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam) {
	MSG msg;
	msg.hwnd = _hwnd;
	msg.lParam = _lParam;
	msg.wParam = _wParam;
	msg.message = _uMsg;
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
HWND createWindow(const Vec2u& _pos, const Vec2u& _size, const char* _windowName) {
	if (!sIsWindowClassRegistered)
		registerClass();

	// Create a windown through the windows API
	HWND mWinapiHandle = CreateWindow("RevWindowClass",	// Class name, registered by the video driver
		_windowName,								// Window name (currently unsupported
		WS_SIZEBOX | WS_CAPTION | WS_POPUP | WS_VISIBLE,	// Creation options
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

	return mWinapiHandle;
}

//--------------------------------------------------------------------------------------------------------------
int main() {
	auto applicationWindow = createWindow(
		{40, 40},
		{1280, 720},
		"EEVO Player test"
	);
	rev::Player player;
	if(!player.init(applicationWindow)) {
		return -1;
	}
	for(;;) {
		if(!player.update())
			return 0;
	}
	return -2;
}

#endif // _WIN32