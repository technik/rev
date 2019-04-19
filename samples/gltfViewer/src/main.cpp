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
#include <graphics/backend/commandPool.h>
#include <graphics/backend/fence.h>
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
			maxVideoMemory = physicalDevices[i].dedicatedVideoMemory;
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

	DoubleBufferSwapChain* swapChain = gfxDevice->createSwapChain(nativeWindow, 0, swapChainInfo);

	// Create two command lists, for alternate frames
	int backBufferIndex = 0;
	CommandPool* cmdPools[2];
	cmdPools[0] = gfxDevice->createCommandPool(CommandList::Graphics);
	cmdPools[1] = gfxDevice->createCommandPool(CommandList::Graphics);
	CommandList* cmdList = gfxDevice->createCommandList(CommandList::Graphics, *cmdPools[0]);
	GpuBuffer* backBuffers[2];
	backBuffers[0] = swapChain->backBuffer(0);
	backBuffers[1] = swapChain->backBuffer(1);
	auto& graphicsQueue = gfxDevice->commandQueue(0);
	Fence* mRenderFence = gfxDevice->createFence();
	uint64_t fenceValues[2] = {};

	// Create command list for copying data
	CommandPool* copyCommandPool = gfxDevice->createCommandPool(CommandList::Copy);
	copyCommandPool->reset();
	CommandList* copyCmdList = gfxDevice->createCommandList(CommandList::Copy, *copyCommandPool);
	copyCmdList->reset(*copyCommandPool);

	// Create a staging buffer
	auto* stagingBuffer = gfxDevice->createCommitedResource(Device::BufferType::Upload, Device::ResourceFlags::None, 4 * sizeof(rev::math::Vec3f));
	auto* vtxBuffer = gfxDevice->createCommitedResource(Device::BufferType::Resident, Device::ResourceFlags::None, 4 * sizeof(rev::math::Vec3f));

	rev::math::Vec3f vtx[4] = {
		rev::math::Vec3f(1.f,0.f,0.f),
		rev::math::Vec3f(0.f,1.f,0.f),
		rev::math::Vec3f(0.f,0.f,1.f),
		rev::math::Vec3f(1.f,1.f,1.f)
	};
	copyCmdList->uploadBufferContent(*vtxBuffer, *stagingBuffer, 4 * sizeof(rev::math::Vec3f), vtx);
	copyCmdList->close();
	auto& copyQueue = gfxDevice->commandQueue(2);
	copyQueue.executeCommandList(*copyCmdList);
	auto copyFenceValue = copyQueue.signalFence(*mRenderFence);
	mRenderFence->waitForValue(copyFenceValue);

	// --- Init other systems ---
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
		CommandPool* cmdPool = cmdPools[backBufferIndex];
		GpuBuffer* backBuffer = backBuffers[backBufferIndex];
		cmdPool->reset();
		cmdList->reset(*cmdPool);
		cmdList->resourceBarrier(backBuffers[backBufferIndex], CommandList::Barrier::Transition, CommandList::ResourceState::Present, CommandList::ResourceState::RenderTarget);
		cmdList->clearRenderTarget(swapChain->renderTarget(backBufferIndex), Vec4f(0.f,1.f,0.f,1.f));
		cmdList->resourceBarrier(backBuffers[backBufferIndex], CommandList::Barrier::Transition, CommandList::ResourceState::RenderTarget, CommandList::ResourceState::Present);
		cmdList->close();

		graphicsQueue.executeCommandList(*cmdList);
		
		fenceValues[backBufferIndex] = graphicsQueue.signalFence(*mRenderFence);
		swapChain->present();

		backBufferIndex = swapChain->getCurrentBackBuffer();

		mRenderFence->waitForValue(fenceValues[backBufferIndex]);
	}
	return -2;
}

#endif // _WIN32