//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
// Engine's default player's entry point
#ifdef _WIN32
#include "player.h"

#include <Windows.h>
#include <graphics/backend/Windows/windowsPlatform.h>
#include <graphics/backend/DirectX12/directX12Driver.h>
#include <math/algebra/vector.h>
#include <input/pointingInput.h>
#include <input/keyboard/keyboardInput.h>
#include <core/platform/osHandler.h>
#include <core/platform/fileSystem/fileSystem.h>

using namespace rev::math;
using namespace rev::gfx;

//--------------------------------------------------------------------------------------------------------------
rev::Player* g_player = nullptr;

//--------------------------------------------------------------------------------------------------------------
bool processWindowsMsg(MSG _msg) {

	if(g_player)
	{
		if(_msg.message == WM_SIZING || _msg.message == WM_SIZE)
		{
			// Get new rectangle size without borders
			RECT clientSurface;
			GetClientRect(_msg.hwnd, &clientSurface);
			auto newSize = Vec2u(clientSurface.right, clientSurface.bottom);
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
	std::string environment;
	unsigned sx = 640;
	unsigned sy = 480;
	float fov = 45.f;

	int process(const std::vector<std::string>& args, int i)
	{
		auto& arg = args[i];
		if(arg == "-env") {
			environment = args[i+1];
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

	// Init graphics
	GraphicsDriver* gfxDriver = new DirectX12Driver();
	const int MAX_GRAPHICS_CARDS = 5;
	GraphicsDriver::PhysicalDeviceInfo physicalDevices[MAX_GRAPHICS_CARDS];
	int numGfxCards = gfxDriver->enumeratePhysicalDevices(physicalDevices, MAX_GRAPHICS_CARDS);

	size_t maxVideoMemory = 0;
	int bestDevice = -1;
	for(int i = 0; i < numGfxCards; ++i)
	{
		if(physicalDevices[i].dedicatedVideoMemory > maxVideoMemory)
		{
			bestDevice = i;
		}
	}

	if(bestDevice < 0)
	{
		std::cout << "Unable to find a suitable graphics card\n";
		return -1;
	}

	GraphicsDriver::PhysicalDevice* gfxCard = gfxDriver->createPhysicalDevice(bestDevice);

	// Create one command queue of each type, all with the same prioriry
	CommandQueue::Info commandQueues[3];
	commandQueues[0].type = CommandQueue::Graphics;
	commandQueues[0].priority = CommandQueue::Normal;
	commandQueues[1].type = CommandQueue::Compute;
	commandQueues[1].priority = CommandQueue::Normal;
	commandQueues[2].type = CommandQueue::Copy;
	commandQueues[2].priority = CommandQueue::Normal;

	Device* gfxDevice = gfxDriver->createDevice(*gfxCard, 3, commandQueues);
	if(!gfxDevice)
	{
		std::cout << "Unable to create graphics device\n";
		return -1;
	}

	// Swap chain creation
	auto windowSize = Vec2u(params.sx, params.sy);
	auto nativeWindow = rev::gfx::createWindow(
		{80, 80},
		windowSize,
		params.scene.empty()?"Rev Player":params.scene.c_str(),
		true,
		true // Visible
	);

	DoubleBufferSwapChain::Info swapChainInfo;
	swapChainInfo.pixelFormat.channel = Image::ChannelFormat::Byte;
	swapChainInfo.pixelFormat.numChannels = 4;
	swapChainInfo.size = windowSize;

	gfxDevice->createSwapChain(nativeWindow, 0, swapChainInfo);





	

	*rev::core::OSHandler::get() += processWindowsMsg;
	
	rev::input::PointingInput::init();
	rev::input::KeyboardInput::init();

	rev::Player player;
	g_player = &player;
	if(!player.init(windowSize, params.scene, params.environment)) {
		return -1;
	}
	for(;;) {
		rev::input::KeyboardInput::get()->refresh();
		if(!rev::core::OSHandler::get()->update())
			return 0;
		if(!player.update())
			return 0;

		// Render
	}
	return -2;
}

#endif // _WIN32