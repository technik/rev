//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
// Little shadertoy implementation
#include <core/platform/fileSystem/fileSystem.h>
#include <core/platform/osHandler.h>
#include <graphics/backend/commandBuffer.h>
#include <graphics/backend/device.h>
#include <graphics/backend/Windows/windowsPlatform.h>
#include <graphics/backend/OpenGL/deviceOpenGLWindows.h>
#include <graphics/renderer/renderPass/fullScreenPass.h>

#include <string>
#include <vector>

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
	Vec2u windowSize = { 200, 200 };
	auto wnd = createWindow(windowStart, windowSize, "ShaderToy", true);

	// Init graphics
	auto gfxDevice = DeviceOpenGLWindows(wnd, true);
	auto& renderQueue = gfxDevice.renderQueue();

	// Renderpass
	RenderPass::Descriptor fullScreenDesc;
	float grey = 0.5f;
	fullScreenDesc.clearColor = { grey,grey,grey, 1.f };
	fullScreenDesc.clearFlags = RenderPass::Descriptor::Clear::Color;
	fullScreenDesc.target = gfxDevice.defaultFrameBuffer();
	fullScreenDesc.viewportSize = windowSize;

	FullScreenPass renderPass(gfxDevice, fullScreenDesc);

	*OSHandler::get() += [&](MSG _msg) {
		if(_msg.message == WM_SIZING || _msg.message == WM_SIZE)
		{
			// Get new rectangle size without borders
			RECT clientSurface;
			GetClientRect(_msg.hwnd, &clientSurface);
			windowSize = Vec2u(clientSurface.right, clientSurface.bottom);
			renderPass.onResizeTarget(windowSize);
			return true;
		}

		//if(rev::input::KeyboardInput::get()->processWin32Message(_msg))
		//	return true;
		return false;
	};

	// Actual shader code
	const string fullScreenCode = R"(
#ifdef PXL_SHADER

layout(location = 0) uniform vec4 t;
layout(location = 1) uniform vec4 Window;

vec3 shade () {	
	vec2 uv = gl_FragCoord.xy / Window.xy;
	return vec3(uv.x,uv.y,sin(t.y));
}

#endif
)";
	renderPass.setPassCode(fullScreenCode.c_str());

	// Command buffer with chaning uniforms
	CommandBuffer::UniformBucket timeUniform;
	
	// Main loop
	float t = 0; // t modulo seconds
	float T = 0; // Total T
	unsigned numSeconds = 0;
	for(;;)
	{
		if(!rev::core::OSHandler::get()->update())
			break;

		// Modify the uniform command
		auto tVector = Vec4f(t,T,t*t,sin(Pi*t));
		timeUniform.clear();
		timeUniform.vec4s.push_back({0, tVector});
		timeUniform.vec4s.push_back({1, {float(windowSize.x()), float(windowSize.y()), 0.f, 0.f}});

		renderPass.render(timeUniform);
		renderPass.submit();

		// Finish frame
		renderQueue.present();

		// Update time
		t += 1.f/60;
		if(t > 1)
		{
			t -= 1.f;
			numSeconds++;
		}
		T = t + numSeconds;
	}

	// Clean up
	FileSystem::end();
	return 0;
}
