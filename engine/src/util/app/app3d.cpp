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

using namespace rev::core;
#ifndef ANDROID
using namespace rev::input;
#endif // !ANDROID
using namespace rev::video;

namespace rev {

#ifndef ANDROID
	//------------------------------------------------------------------------------------------------------------------
	App3d::App3d(int _argc, const char** _argv)
		: mEngine(_argc,_argv)
	{
		mDriver = new GraphicsDriver(mEngine.mainWindow());
		mDriver->setClearColor(Color(0.7f));

		mKeyboard = input::KeyboardInput::get();
		mWindow = mEngine.mainWindow();
		// Init renderer
	}
#endif // !ANDROID

#ifdef ANDROID
	//------------------------------------------------------------------------------------------------------------------
	App3d::App3d(ANativeActivity* _activity)
		:mEngine(_activity)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	void App3d::initGraphics(ANativeWindow* _window) {
		GraphicsDriver* driver = new GraphicsDriver();
		driver->setWindow(_window);
		mDriver = driver;
		mShader = Shader::manager()->get("solid");
		driver->setShader(mShader);
	}
#endif // ANDROID

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