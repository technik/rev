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
#include <core/time/time.h>

#include <graphics/backend/device.h>
#include <graphics/backend/DirectX12/directX12Driver.h>
#include <graphics/backend/doubleBufferSwapChain.h>

using namespace rev::gfx;

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
			core::Time::get()->update();
			auto dt = core::Time::get()->frameTime();
			//	updateLogic
			if (!updateLogic(dt))
				return;
			//	render
			render();
		}
		end();
	}

	//------------------------------------------------------------------------------------------------
	void Base3dApplication::initEngineCore()
	{
		core::Time::init();
		rev::core::FileSystem::init();
	}

	//------------------------------------------------------------------------------------------------
	bool Base3dApplication::initGraphics()
	{
		if (!initGraphicsDevice())
			return false;

		createSwapChain();
		return true;
	}

	//------------------------------------------------------------------------------------------------
	bool Base3dApplication::initGraphicsDevice()
	{
		DirectX12Driver gfxDriver;
		constexpr int MAX_GRAPHICS_CARDS = 5;
		GraphicsDriver::PhysicalDeviceInfo physicalDevices[MAX_GRAPHICS_CARDS];
		int numGfxCards = gfxDriver.enumeratePhysicalDevices(physicalDevices, MAX_GRAPHICS_CARDS);

		size_t maxVideoMemory = 0;
		int bestDevice = -1;
		for (int i = 0; i < numGfxCards; ++i)
		{
			if (physicalDevices[i].dedicatedVideoMemory > maxVideoMemory)
			{
				bestDevice = i;
				maxVideoMemory = physicalDevices[i].dedicatedVideoMemory;
			}
		}

		if (bestDevice < 0)
		{
			std::cout << "Unable to find a suitable graphics card\n";
			return false;
		}

		GraphicsDriver::PhysicalDevice* gfxCard = gfxDriver.createPhysicalDevice(bestDevice);

		// Create one command queue of each type, all with the same prioriry
		CommandQueue::Info commandQueues[3];
		commandQueues[0].type = CommandQueue::Graphics;
		commandQueues[0].priority = CommandQueue::Normal;
		commandQueues[1].type = CommandQueue::Compute;
		commandQueues[1].priority = CommandQueue::Normal;
		commandQueues[2].type = CommandQueue::Copy;
		commandQueues[2].priority = CommandQueue::Normal;

		m_gfxDevice = gfxDriver.createDevice(*gfxCard, 3, commandQueues);
		if (!m_gfxDevice)
		{
			std::cout << "Unable to create graphics device\n";
			return false;
		}

		return true;
	}

	void Base3dApplication::createSwapChain()
	{
		auto nativeWindow = rev::gfx::createWindow(
			{ 80, 80 },
			m_windowSize,
			"Rev Player",
			true,
			true // Visible
		);

		DoubleBufferSwapChain::Info swapChainInfo;
		swapChainInfo.pixelFormat.componentType = ScalarType::uint8;
		swapChainInfo.pixelFormat.components = 4;
		swapChainInfo.size = m_windowSize;

		m_swapChain = m_gfxDevice->createSwapChain(nativeWindow, 0, swapChainInfo);

		m_backBuffers[0] = m_swapChain->backBuffer(0);
		m_backBuffers[1] = m_swapChain->backBuffer(1);
	}
}