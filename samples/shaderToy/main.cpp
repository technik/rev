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

using namespace rev::math;
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
	ShaderToy(int _argc, const char** _argv)
		: App3d(_argc, _argv)
		, mTime(0.f)
	{
		rev::video::GraphicsDriver& driver = driver3d();
		driver.setAttribBuffer(0, 4, vertices);
		uTime = driver.getUniformLocation("uTime");
		uResolution = driver.getUniformLocation("uResolution");
		mShader = Shader::manager()->get("shader");
	}

private:
	int uTime;
	int uResolution;
	Shader::Ptr	mShader;
	float mTime;

	bool frame(float _dt) override {

		rev::video::GraphicsDriver& driver = driver3d();
		driver.setShader((Shader*)mShader);
		
		// Update time
		mTime += _dt;
		driver.setUniform(uTime, mTime);

		// Keep window resolution updated
		Vec2u resolution = window().size();
		driver.setUniform(uResolution, Vec2f(float(resolution.x), float(resolution.y)));
		
		driver.drawIndexBuffer(6, indices, GraphicsDriver::EPrimitiveType::triangles);
		return true;
	}
};

// ---- Generic main loop ----
int main(int _argc, const char** _argv) {
	
	ShaderToy app(_argc,_argv);

	while(app.update());
	return 0;
}