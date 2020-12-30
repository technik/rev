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
#include <chrono>

#include <core/platform/cmdLineParser.h>
#include <core/platform/fileSystem/fileSystem.h>
#include <core/platform/osHandler.h>

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
		std::chrono::high_resolution_clock::time_point lastTime = m_appTime.now();
		// Parse command line
		core::CmdLineParser arguments;
		arguments.addFlag("fullscreen", m_fullScreen);
		getCommandLineOptions(arguments);
		arguments.parse(argc, argv);
		// Init engine
		initEngineCore();
		if (!initGraphics())
			return;
		// Init application
		init();

		// main loop
		const int kMaxIter = 4;
		const TimeDelta fixedDt = TimeDelta(1.f / 60);
		const TimeDelta maxCarryOverTime = TimeDelta(0.5f);
		std::chrono::duration<float> accumTime = fixedDt; // Start with a regular frame
		for (;;)
		{
			if (!core::OSHandler::get()->update())
				break;
			
			// Fixed time simulation loop and render
			for(int i = 0; i < kMaxIter &&  accumTime >= fixedDt; ++i)
			{
				accumTime -= fixedDt;
				//	updateLogic
				if (!updateLogic(fixedDt))
					return;
			}
			//	render
			render(fixedDt);

			// Prepare next frame's time
			auto t = m_appTime.now();
			accumTime += t - lastTime; // Accumulate this frame's duration plus left over time from this frame
			accumTime = std::min(maxCarryOverTime, accumTime); // Clamp max carry over
			lastTime = t;
		}
		end();
		core::FileSystem::end();
	}

	//------------------------------------------------------------------------------------------------
	void Base3dApplication::initEngineCore()
	{
		core::OSHandler::startUp();
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
		SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);

		Vec2u windowStart = { 100, 150 };
		if (m_fullScreen)
		{
			m_windowSize.x() = GetSystemMetrics(SM_CXSCREEN);
			m_windowSize.y() = GetSystemMetrics(SM_CYSCREEN);
		}
		auto wnd = createWindow(windowStart, m_windowSize, this->name().c_str(), m_fullScreen, true, true);
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
			if (_msg.message == WM_DPICHANGED)
			{
				const RECT* recommendedSurface = reinterpret_cast<const RECT*>(_msg.lParam);
				m_windowSize = Vec2u(
					recommendedSurface->right - recommendedSurface->left,
					recommendedSurface->bottom - recommendedSurface->top);
				SetWindowPos(wnd,
					NULL,
					recommendedSurface->left,
					recommendedSurface->top,
					m_windowSize.x(),
					m_windowSize.y(),
					SWP_NOZORDER | SWP_NOACTIVATE);

				onResize();
				return true;
			}

			return false;
		};

		return true;
	}
}