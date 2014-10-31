//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
// on 2014-10-27
//----------------------------------------------------------------------------------------------------------------------
// Engine's hello world (simple triangle)

#include <core/memory/newAllocator.h>
#include <engine.h>
#include <input/keyboard/keyboardInput.h>
#include <math/algebra/vector.h>
#include <vector>
#include <video/basicTypes/color.h>
#include <video/graphics/driver/graphicsDriver.h>
#include <video/graphics/driver/openGL/openGLDriver.h>

int main(int _argc, const char** _argv) {
	rev::Engine<rev::core::NewAllocator>	mEngine(_argc,_argv);

	//rev::input::KeyboardInput*				mKeyboard;
	rev::video::GraphicsDriver*				mDriver;

	mDriver = new rev::video::OpenGLDriver(mEngine.mainWindow());
	mDriver->setClearColor(rev::video::Color(0.7f));
	//mKeyboard = rev::input::KeyboardInput::get();
	//mEngine.mainWindow();

	while(1) {
		mDriver->clearColorBuffer();
		mDriver->clearZBuffer();
		mDriver->finishFrame();
	}
	return 0;
}