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

#include <string>
#include <vector>

#include "gfx/renderer.h"
#include "gfx/world.h"

using namespace std;
using namespace rev::core;
using namespace rev::gfx;
using namespace rev::math;

//--------------------------------------------------------------------------------------------------------------
int main(int _argc, const char** _argv) {
	// Init engine core systems
	OSHandler::startUp();
	FileSystem::init();

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

		//if(rev::input::PointingInput::get()->processMessage(_msg))
		//	return true;
		//if(rev::input::KeyboardInput::get()->processWin32Message(_msg))
		//	return true;
		return false;
	};
	
	vkft::gfx::World world;
	rev::gfx::Camera camera;

	// Main loop
	float t = 0;
	for(;;)
	{
		if(!rev::core::OSHandler::get()->update())
			break;

		// Modify the uniform command
		//Vec3f color = Vec3f(t,t,t);
		//timeUniform.vec3s.push_back({0, color});
		//uniformCmd.clear();
		//uniformCmd.setUniformData(timeUniform);

		// Send pass to the GPU
		renderer.render(world, camera);

		// Update time
		t += 1.f/60;
		if(t > 1) t -= 1.f;
	}

	// Clean up
	FileSystem::end();
	return 0;
}
