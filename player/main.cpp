//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
// Engine's default player's entry point
#ifdef _WIN32
#include "player.h"

#include <Windows.h>
#include <math/algebra/vector.h>
#include <input/pointingInput.h>

using namespace rev::math;

//--------------------------------------------------------------------------------------------------------------
static bool sIsWindowClassRegistered = false;

bool processWindowsMsg(MSG _msg) {
	/*
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
	}*/
	if(rev::input::PointingInput::get()->processMessage(_msg))
		return true;
	return false;
}

//--------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowProc(HWND _hwnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam) {
	MSG msg;
	msg.hwnd = _hwnd;
	msg.lParam = _lParam;
	msg.wParam = _wParam;
	msg.message = _uMsg;
	if(processWindowsMsg(msg))
		return 0;
	return DefWindowProc(_hwnd, _uMsg, _wParam, _lParam);
}

//--------------------------------------------------------------------------------------------------------------
bool processSystemMessages() {
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		// If exit requested, don't bother processing anything else
		if (msg.message == WM_QUIT || msg.message == WM_CLOSE)
			return false;
		else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return true;
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
		"Rev Player"
	);
	rev::input::PointingInput::init();
	rev::Player player;
	if(!player.init(applicationWindow)) {
		return -1;
	}
	for(;;) {
		if(!processSystemMessages())
			return 0;
		if(!player.update())
			return 0;
	}
	return -2;
}

#endif // _WIN32