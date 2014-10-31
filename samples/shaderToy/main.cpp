//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
// on 2014-10-31
//----------------------------------------------------------------------------------------------------------------------
// Sample to play with shaders

#include <engine.h>
#include <input/keyboard/keyboardInput.h>
#include <math/algebra/vector.h>
#include <vector>
#include <util/app/app3d.h>
#include <video/basicTypes/color.h>
#include <video/graphics/driver/graphicsDriver.h>
#include <video/graphics/driver/openGL/openGLDriver.h>

using namespace rev::video;

// --- The triangle ---
const Vec3f vertices[] = {
	{-1.f, -1.f, 0.f },
	{1.f, -1.f, 0.f },
	{1.f, 1.f, 0.f },
	{-1.f, 1.f, 0.f },
};

uint16_t indices[] = { 0, 1, 2, 2, 3, 0 };

// ---- Actual application code
class ShaderToy : public rev::App3d {
public:
	ShaderToy(int _argc, const char** _argv) : App3d(_argc, _argv) {
		driver3d().setAttribBuffer(0, 4, vertices);
		mShader = Shader::manager()->get("shader");
	}

private:
	Shader::Ptr	mShader;

	bool frame(float) override {
		driver3d().setShader((Shader*)mShader);
		driver3d().drawIndexBuffer(6, indices, GraphicsDriver::EPrimitiveType::triangles);
		return true;
	}
};

// ---- Generic main loop ----
int main(int _argc, const char** _argv) {
	
	ShaderToy app(_argc,_argv);

	while(app.update());
	return 0;
}