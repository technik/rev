//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
// on 2014-10-31
//----------------------------------------------------------------------------------------------------------------------
// Base class for 3d applications
#include "app3d.h"

#include <video/basicTypes/color.h>
#include <video/graphics/driver/openGL/openGLDriver.h>

using namespace rev::core;
using namespace rev::input;
using namespace rev::video;

namespace rev {

	//------------------------------------------------------------------------------------------------------------------
	App3d::App3d() : App3d(0,nullptr) {
	}

	//------------------------------------------------------------------------------------------------------------------
	App3d::App3d(int _argc, const char** _argv)
		: mEngine(_argc,_argv)
	{
		mDriver = new OpenGLDriver(mEngine.mainWindow());
		mDriver->setClearColor(Color(0.7f));
		mShader = Shader::manager()->get("shader");
		mDriver->setShader((Shader*)mShader);

		mKeyboard = input::KeyboardInput::get();
		mWindow = mEngine.mainWindow();
	}

	//------------------------------------------------------------------------------------------------------------------
	bool App3d::update() {
		preFrame();

		if(mKeyboard->pressed(input::KeyboardInput::eEscape))
			return false;

		if(frame(core::Time::get()->frameTime())) {
			postFrame();
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
	}

	//------------------------------------------------------------------------------------------------------------------
	void App3d::postFrame() {
		mDriver->finishFrame();
	}

}	// namespace rev