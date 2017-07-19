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
#include <cjson/json.h>
#include <fstream>

using namespace rev::core;
#ifndef ANDROID
using namespace rev::input;
#endif // !ANDROID
using namespace rev::video;

namespace rev {

	//------------------------------------------------------------------------------------------------------------------
	App3d::App3d(const StartUpInfo& _info)
		:mEngine(_info)
	{
		mMinFrameTime = 1.f/120;
		// Open config file
		cjson::Json config;// = loadConfig();
#ifdef ANDROID
		GraphicsDriver* driver = new GraphicsDriver(config);
		//driver->setWindow(_info.activity->);
		mDriver = driver;
#else
#ifdef OPENGL_45
		mDriver = new GraphicsDriver(mEngine.mainWindow(), config);
#endif // OPENGL_45

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

		if (!mEngine.update())
			return false;

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
		if(!frame(dt))
			return false;
#ifdef OPENGL_45
		mDriver->finishFrame();
#endif OPENGL_45
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool App3d::frame(float) {
		return true;
	}

}	// namespace rev