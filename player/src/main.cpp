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



struct CmdLineParams
{
	std::string scene;
	std::string background;
	unsigned sx = 640;
	unsigned sy = 480;
	float fov = 45.f;

	int process(const std::vector<std::string>& args, int i)
	{
		auto& arg = args[i];
		if(arg == "-bg") {
			background = args[i+1];
			return 2;
		}
		if(arg == "-scene")
		{
			scene = args[i+1];
			return 2;
		}
		if(arg == "-w")
		{
			sx = atoi(args[i+1].c_str());
			return 2;
		}
		if(arg == "-h")
		{
			sy = atoi(args[i+1].c_str());
			return 2;
		}
		if(arg == "-fov")
		{
			fov = (float)atof(args[i+1].c_str());
			return 2;
		}
		if(arg == "-fullHD")
		{
			sx = 1920;
			sy = 1080;
			return 1;
		}
		return 1;
	}

	CmdLineParams(int _argc, const char** _argv)
	{
		std::vector<std::string> args(_argc);
		// Read all params
		int i = 0;
		for(auto& s : args)
			s = _argv[i++];
		i = 0;
		while(i < _argc)
			i += process(args, i);
	}
};

//--------------------------------------------------------------------------------------------------------------
int main(int _argc, const char** _argv) {

	CmdLineParams params(_argc,_argv);

	rev::core::OSHandler::startUp();
	rev::core::FileSystem::init();

	auto nativeWindow = rev::graphics::WindowWin32::createWindow(
		{40, 40},
		{params.sx, params.sy},
		params.scene.empty()?"Rev Player":params.scene.c_str()
	);

	*rev::core::OSHandler::get() += processWindowsMsg;
	
	rev::input::PointingInput::init();
	rev::input::KeyboardInput::init();
	rev::Player player;
	g_player = &player;
	if(!player.init(&nativeWindow, params.scene, params.background)) {
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