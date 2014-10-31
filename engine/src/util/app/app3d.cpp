//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
// on 2014-10-31
//----------------------------------------------------------------------------------------------------------------------
// Base class for 3d applications
#include "app3d.h"

#include <video/basicTypes/color.h>
#include <video/graphics/driver/openGL/openGLDriver.h>

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
	}

	//------------------------------------------------------------------------------------------------------------------
	bool App3d::update() {
		preFrame();
		if(frame(0.f)) { /// 666 TODO: Support time
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
		mDriver->clearColorBuffer();
		mDriver->clearZBuffer();
	}

	//------------------------------------------------------------------------------------------------------------------
	void App3d::postFrame() {
		mDriver->finishFrame();
	}

}	// namespace rev