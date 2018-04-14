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
#include <core/platform/osHandler.h>
#include <core/platform/fileSystem/fileSystem.h>

using namespace rev::math;

//--------------------------------------------------------------------------------------------------------------
rev::Player* g_player = nullptr;

//--------------------------------------------------------------------------------------------------------------
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
int main() {
	rev::core::OSHandler::startUp();
	rev::core::FileSystem::init();

	auto nativeWindow = rev::graphics::WindowWin32::createWindow(
		{40, 40},
		{1024, 640},
		"Rev Player"
	);

	*rev::core::OSHandler::get() += processWindowsMsg;
	
	rev::input::PointingInput::init();
	rev::input::KeyboardInput::init();
	rev::Player player;
	g_player = &player;
	if(!player.init(&nativeWindow)) {
		return -1;
	}
	for(;;) {
		rev::input::KeyboardInput::get()->refresh();
		if(!rev::core::OSHandler::get()->update())
			return 0;
		if(!player.update())
			return 0;
	}
	return -2;
}

#endif // _WIN32