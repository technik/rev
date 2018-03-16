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
#include <input/keyboard/keyboardInput.h>

using namespace rev::math;

//--------------------------------------------------------------------------------------------------------------
rev::Player* g_player = nullptr;

bool processWindowsMsg(MSG _msg) {

	if(g_player)
	{
		if(_msg.message == WM_SIZE)
		{
			LPARAM lparam = _msg.lParam;
				g_player->onWindowResize(Vec2u(LOWORD(lparam), HIWORD(lparam)));
			return true;
		}
		if(_msg.message == WM_SIZING)
		{
			RECT* rect = reinterpret_cast<RECT*>(_msg.lParam);
			auto newSize = Vec2u(rect->right-rect->left, rect->bottom-rect->top);
			g_player->onWindowResize(newSize);
			return true;
		}
	}

	if(rev::input::PointingInput::get()->processMessage(_msg))
		return true;
	if(rev::input::KeyboardInput::get()->processWin32Message(_msg))
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
int main() {
	auto nativeWindow = rev::graphics::WindowWin32::createWindow(
		{40, 40},
		{1024, 640},
		"Rev Player"
	);
	rev::input::PointingInput::init();
	rev::input::KeyboardInput::init();
	rev::Player player;
	g_player = &player;
	if(!player.init(&nativeWindow)) {
		return -1;
	}
	for(;;) {
		rev::input::KeyboardInput::get()->refresh();
		if(!processSystemMessages())
			return 0;
		if(!player.update())
			return 0;
	}
	return -2;
}

#endif // _WIN32