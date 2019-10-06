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

#include <input/pointingInput.h>
#include <input/keyboard/keyboardInput.h>

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
		init();
		// main loop
		// TODO: Think of simulation loop and deal with variable render times and fixed simulation time
		const float fixedDt = 1.f / 60;
		for (;;)
		{
			if (!core::OSHandler::get()->update())
				break;
			
			core::Time::get()->update();
			auto frameTime = core::Time::get()->frameTime();
			frameTime = std::min(frameTime, 1.f); // Clamp time to at most 1 second
			m_accumTime += frameTime;
			while (m_accumTime > fixedDt)
			{
				m_accumTime -= fixedDt;
				//	updateLogic
				if (!updateLogic(fixedDt))
					return;
			}
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

		// Init input systems
		rev::input::KeyboardInput::init();
		rev::input::PointingInput::init();

		*core::OSHandler::get() += [&](MSG _msg) {
			if (rev::input::PointingInput::get()->processMessage(_msg))
				return true;
			if (rev::input::KeyboardInput::get()->processWin32Message(_msg))
				return true;
			return false;
		};
	}

	//------------------------------------------------------------------------------------------------
	bool Base3dApplication::initGraphics()
	{
		Vec2u windowStart = { 100, 150 };
		Vec2u windowSize = { 200, 200 };
		auto wnd = createWindow(windowStart, windowSize, this->name().c_str(), true);
		auto openglDevice = std::make_unique<DeviceOpenGLWindows>(wnd, true);
		if (!openglDevice)
		{
			std::cout << "Unable to create graphics device\n";
			return false;
		}

		m_backBuffer = openglDevice->defaultFrameBuffer();
		m_gfxDevice = std::move(openglDevice);

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

			return false;
		};

		return true;
	}
}