//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2019 Carmelo J Fdez-Aguera
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#include "base3dApplication.h"

#include <iostream>

#include <core/platform/cmdLineParser.h>
#include <core/platform/fileSystem/fileSystem.h>
#include <core/platform/osHandler.h>
#include <core/time/time.h>

#include <graphics/backend/device.h>
#include <graphics/backend/OpenGL/deviceOpenGLWindows.h>

using namespace rev::gfx;
using namespace rev::math;

namespace rev::game {

	//------------------------------------------------------------------------------------------------
	Base3dApplication::~Base3dApplication()
	{
		// Out of line constructor allows use of unique_ptr with prototypes only on the header file
	}

	//------------------------------------------------------------------------------------------------
	void Base3dApplication::run(int argc, const char** argv)
	{
		// Parse command line
		core::CmdLineParser arguments;
		getCommandLineOptions(arguments);
		arguments.parse(argc, argv);
		// Init engine
		initEngineCore();
		if (!initGraphics())
			return;
		// Init application
		init(arguments);
		// main loop
		// TODO: Think of simulation loop and deal with variable render times and fixed simulation time
		float dt = 1 / 60.f;
		for (;;)
		{
			core::OSHandler::get()->update();
			core::Time::get()->update();
			auto dt = core::Time::get()->frameTime();
			//	updateLogic
			if (!updateLogic(dt))
				return;
			//	render
			render();
		}
		end();
		core::FileSystem::end();
	}

	//------------------------------------------------------------------------------------------------
	void Base3dApplication::initEngineCore()
	{
		core::OSHandler::startUp();
		core::Time::init();
		core::FileSystem::init();
	}

	//------------------------------------------------------------------------------------------------
	bool Base3dApplication::initGraphics()
	{
		return initGraphicsDevice();

		// Hook up window resizing callback
		*core::OSHandler::get() += [&](MSG _msg) {
			if (_msg.message == WM_SIZING || _msg.message == WM_SIZE)
			{
				// Get new rectangle size without borders
				RECT clientSurface;
				GetClientRect(_msg.hwnd, &clientSurface);
				m_windowSize = Vec2u(clientSurface.right, clientSurface.bottom);
				
				onResize();
				return true;
			}

			//if(rev::input::KeyboardInput::get()->processWin32Message(_msg))
			//	return true;
			return false;
		};
	}

	//------------------------------------------------------------------------------------------------
	bool Base3dApplication::initGraphicsDevice()
	{
		Vec2u windowStart = { 100, 150 };
		Vec2u windowSize = { 200, 200 };
		auto wnd = createWindow(windowStart, windowSize, "Application", true);
		auto openglDevice = std::make_unique<DeviceOpenGLWindows>(wnd, true);
		if (!openglDevice)
		{
			std::cout << "Unable to create graphics device\n";
			return false;
		}

		m_backBuffer = openglDevice->defaultFrameBuffer();
		m_gfxDevice = std::move(openglDevice);

		return true;
	}
}