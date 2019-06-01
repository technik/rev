//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
// Engine's default player's entry point
#ifdef _WIN32
#include "player.h"

#include <Windows.h>
#include <graphics/backend/Windows/windowsPlatform.h>
#include <graphics/backend/DirectX12/d3dx12.h>
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

//--------------------------------------------------------------------------------------------------------------
int main(int _argc, const char** _argv) {

	rev::Player::CmdLineOptions params(_argc,_argv);

	// --- Init other systems ---
	rev::core::OSHandler::startUp();
	*rev::core::OSHandler::get() += processWindowsMsg;
	rev::input::PointingInput::init();
	rev::input::KeyboardInput::init();

	rev::Player player;
	g_player = &player;
	if(!player.init(params)) {
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

		cmdList->bindPipeline(triShader);
		cmdList->bindAttribute(0, 3*sizeof(Vec3f), sizeof(Vec3f), vtxBuffer);
		cmdList->bindIndexBuffer(3 * sizeof(uint16_t), CommandList::NdxBufferFormat::U16, indexBuffer);
		cmdList->bindRenderTarget(swapChain->renderTarget(backBufferIndex));
		cmdList->setViewport(Vec2u::zero(), params.windowSize);
		cmdList->setScissor(Vec2u::zero(), params.windowSize);
		cmdList->drawIndexed(0, 3, 0);

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