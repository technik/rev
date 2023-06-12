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

#include <gfx/backend/Context.h>
#include <gfx/backend/Vulkan/renderContextVulkan.h>

#include <input/pointingInput.h>
#include <input/keyboard/keyboardInput.h>

using namespace rev::gfx;
using namespace rev::math;

namespace rev::game {

	//------------------------------------------------------------------------------------------------------------------
	void Base3dApplication::CommandLineOptions::registerOptions(core::CmdLineParser& args)
	{
		args.addOption("w", &windowSize.x());
		args.addOption("h", &windowSize.y());
		args.addFlag("fullscreen", fullScreen);
		args.addFlag("dx12", useDX12);
	}

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
		m_options.registerOptions(arguments);
		getCommandLineOptions(arguments);
		arguments.parse(argc, argv);
		// Init engine
		initEngineCore();
		if (!initGraphics(m_options.useDX12))
			return;
		// Init application
		init();

		// OS Hooks
		*core::OSHandler::get() += [&](MSG _msg) {
			if (rev::input::PointingInput::get()->processMessage(_msg))
				return true;
			if (rev::input::KeyboardInput::get()->processWin32Message(_msg))
				return true;
			return false;
		};

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

		// Wait for the GPU to catch up before destroying stuff
		end(); // End derived application

		// Shutdown systems
		endGraphics();
		core::FileSystem::end();
	}

	const math::Vec2u& Base3dApplication::windowSize() const {
		return RenderContext().windowSize();
	}

	//------------------------------------------------------------------------------------------------
	void Base3dApplication::initEngineCore()
	{
		core::OSHandler::startUp();
		core::FileSystem::init();

		// Init input systems
		rev::input::KeyboardInput::init();
		rev::input::PointingInput::init();
	}

	//------------------------------------------------------------------------------------------------
	bool Base3dApplication::initGraphics(bool useDX12)
	{
		if (!Context::initSingleton(name().c_str(), useDX12 ? Context::GfxAPI::DX12 : Context::GfxAPI::Vulkan))
		{
			return false;
		}

		Context::SwapChainOptions swpOptions{};
		swpOptions.fullScreen = m_options.fullScreen;
		swpOptions.vSync = true;

		if (RenderContext().createWindow(
			m_options.windowPosition, m_options.windowSize,
			name().c_str(),
			true, 
			swpOptions))
		{
			m_resizeDelegate = RenderContext().onResize() += [this](math::Vec2u imgSize) { this->onResize(); };
			return true;
		}
		else
		{
			return false;
		}
	}

	//------------------------------------------------------------------------------------------------
	void Base3dApplication::endGraphics()
	{
		RenderContext().destroyWindow();
		Context::endSingleton();
	}
}