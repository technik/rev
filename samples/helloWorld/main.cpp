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
//#include <video/graphics/shader/shader.h>

using namespace rev::video;

int main(int _argc, const char** _argv) {
	rev::Engine<rev::core::NewAllocator>	mEngine(_argc,_argv);

	//rev::input::KeyboardInput*				mKeyboard;
	GraphicsDriver*				mDriver;

	mDriver = new OpenGLDriver(mEngine.mainWindow());
	mDriver->setClearColor(Color(0.7f));
	//mKeyboard = rev::input::KeyboardInput::get();
	Shader::Ptr mShader = Shader::manager()->get("shader");
	mDriver->setShader((Shader*)mShader);
	
	uint16_t indices[3] = { 0, 1, 2 };
	Vec3f vertices[3] = {
		{-1.f, -1.f, 0.f },
		{1.f, -1.f, 0.f },
		{0.f, 1.f, 0.f },
	};

	mDriver->setAttribBuffer(0, 3, vertices);
	//mDriver.setUniform(mColorUniform, _fill);

	for(;;) {
		mDriver->clearColorBuffer();
		mDriver->clearZBuffer();

		// Actual draw
		mDriver->drawIndexBuffer(3, indices, GraphicsDriver::EPrimitiveType::triangles);

		mDriver->finishFrame();
	}
	return 0;
}