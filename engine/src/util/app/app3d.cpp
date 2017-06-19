//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
// on 2014-10-31
//----------------------------------------------------------------------------------------------------------------------
// Base class for 3d applications
#include "app3d.h"

#include <video/basicTypes/color.h>
#include <video/graphics/driver/graphicsDriver.h>
#include <video/window/window.h>
#include <core/time/time.h>
#include <thread>
#include <chrono>
#include <core/platform/platformInfo.h>

using namespace rev::core;
#ifndef ANDROID
using namespace rev::input;
#endif // !ANDROID
using namespace rev::video;

namespace rev {

	//------------------------------------------------------------------------------------------------------------------
	App3d::App3d(const PlatformInfo& _info)
		:mEngine(_info)
	{
		mMinFrameTime = 1.f/120;
#ifdef ANDROID
		GraphicsDriver* driver = new GraphicsDriver();
		//driver->setWindow(_info.activity->);
		mDriver = driver;
#else
		mDriver = new GraphicsDriver(mEngine.mainWindow());

		mKeyboard = input::KeyboardInput::get();
		mWindow = mEngine.mainWindow();
#endif // !ANDROID
	}
	//------------------------------------------------------------------------------------------------------------------
	bool App3d::update() {
#ifdef ANDROID
		if(!mDriver)
			return true;
#endif // ANDROID

		preFrame();

#ifndef ANDROID
		if(mKeyboard->pressed(input::KeyboardInput::eEscape))
			return false;
#endif // !ANDROID

		float dt = core::Time::get()->frameTime();
		if (dt < mMinFrameTime) // Limit framerate
		{
			int timeLeft = (int)(1000.f*(mMinFrameTime - dt));
			std::this_thread::sleep_for(std::chrono::milliseconds(timeLeft));
			dt = mMinFrameTime;
		}
		if(frame(dt)) {
			postFrame();
			//mProfiler.update(dt);
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool App3d::frame(float) {
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	void App3d::preFrame(){
		mEngine.update();
		Time::get()->update();
	}

	//------------------------------------------------------------------------------------------------------------------
	void App3d::postFrame() {
		mDriver->finishFrame();
	}

}	// namespace rev