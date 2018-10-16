//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
// Minecraft-style sample game
//----------------------------------------------------------------------------------------------------------------------
#include <core/platform/fileSystem/fileSystem.h>
#include <core/platform/osHandler.h>
#include <graphics/scene/camera.h>
#include <graphics/backend/device.h>
#include <graphics/backend/Windows/windowsPlatform.h>
#include <graphics/backend/OpenGL/deviceOpenGLWindows.h>

#include <game/scene/camera.h>
#include <game/scene/transform/flyby.h>
#include <game/scene/transform/transform.h>

#include <input/pointingInput.h>
#include <input/keyboard/keyboardInput.h>

#include <string>
#include <vector>

#include "gfx/renderer.h"
#include "gfx/world.h"

using namespace std;
using namespace rev::core;
using namespace rev::gfx;
using namespace rev::math;

//--------------------------------------------------------------------------------------------------------------
void initCamera(rev::game::SceneNode*& camNode, rev::game::FlyBy*& player, rev::gfx::Camera*& gfxCam)
{
	camNode = new rev::game::SceneNode("player camera");
	const float camSpeed = 1.f;
	const float camAngSpeed = 1.f;
	auto xform = camNode->addComponent<rev::game::Transform>();
	player = camNode->addComponent<rev::game::FlyBy>(camSpeed, camAngSpeed);
	xform->xForm.position() = rev::math::Vec3f{0.f, 0.f, 1.f };
	gfxCam = &*camNode->addComponent<rev::game::Camera>()->cam();

	camNode->init();
}

//--------------------------------------------------------------------------------------------------------------
int main(int _argc, const char** _argv) {
	// Init engine core systems
	OSHandler::startUp();
	FileSystem::init();

	// Init input systems
	rev::input::KeyboardInput::init();
	rev::input::PointingInput::init();

	// Create the application window
	Vec2u windowStart = {100, 150};
	Vec2u windowSize = { 400, 300 };
	auto wnd = createWindow(windowStart, windowSize, "Vulkraft", true);

	// Init graphics
	auto gfxDevice = DeviceOpenGLWindows(wnd, true);
	auto& renderQueue = gfxDevice.renderQueue();

	// Init renderer
	vkft::gfx::Renderer renderer(gfxDevice, windowSize);

	*OSHandler::get() += [&renderer](MSG _msg) {
		if(_msg.message == WM_SIZING || _msg.message == WM_SIZE)
		{
			// Get new rectangle size without borders
			RECT clientSurface;
			GetClientRect(_msg.hwnd, &clientSurface);
			auto newSize = Vec2u(clientSurface.right, clientSurface.bottom);
			renderer.onResizeTarget(newSize);
			return true;
		}

		if(rev::input::PointingInput::get()->processMessage(_msg))
			return true;
		if(rev::input::KeyboardInput::get()->processWin32Message(_msg))
			return true;
		return false;
	};
	
	vkft::gfx::World world;

	rev::game::SceneNode* camNode = nullptr;
	rev::game::FlyBy* player = nullptr;
	rev::gfx::Camera* playerCam = nullptr;
	initCamera(camNode, player, playerCam);

	// Main loop
	float t = 0;
	for(;;)
	{
		if(!rev::core::OSHandler::get()->update())
			break;

		camNode->update(1.f/60);
		// Modify the uniform command
		//Vec3f color = Vec3f(t,t,t);
		//timeUniform.vec3s.push_back({0, color});
		//uniformCmd.clear();
		//uniformCmd.setUniformData(timeUniform);

		// Send pass to the GPU
		renderer.render(world, *playerCam);
		gfxDevice.renderQueue().present();

		// Update time
		t += 1.f/60;
		if(t > 1) t -= 1.f;
	}

	// Clean up
	FileSystem::end();
	return 0;
}
