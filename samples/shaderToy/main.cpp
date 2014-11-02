//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
// on 2014-10-31
//----------------------------------------------------------------------------------------------------------------------
// Sample to play with shaders

#include <engine.h>
#include <game/scene/camera/flyByCamera.h>
#include <input/keyboard/keyboardInput.h>
#include <math/algebra/vector.h>
#include <util/app/app3d.h>
#include <video/basicTypes/color.h>
#include <video/graphics/driver/graphicsDriver.h>
#include <video/graphics/driver/openGL/openGLDriver.h>
#include <vector>

using namespace rev::game;
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
		mShader = Shader::manager()->get("shader");
		processArgs(_argc, _argv);
	}

private:
	Shader::Ptr		mShader;
	float			mTime;
	FlyByCamera*	mCam = nullptr;
	
	//----------------------------------------------------------------
	void processArgs(int _argc, const char** _argv) {
		for(int i = 0; i < _argc; ++i) {
			std::string arg(_argv[i]);
			if(arg.substr(0,8)=="-shader=") {
				mShader = Shader::manager()->get(arg.substr(8));
			} else if(arg == "-flyby") {
				mCam = new FlyByCamera(1.57079f, 1.333f, 0.001f, 1000.0f); // Actually, inside the shader we only care for camera's position
			}
		}
	}

	//----------------------------------------------------------------
	bool frame(float _dt) override {

		rev::video::GraphicsDriver& driver = driver3d();
		driver.setShader((Shader*)mShader);
		driver.setAttribBuffer(0, 4, vertices);
		int uTime = driver.getUniformLocation("uTime");
		int uResolution = driver.getUniformLocation("uResolution");
		int uCamPos = driver.getUniformLocation("uCamPos");
		
		// Update time
		mTime += _dt;
		driver.setUniform(uTime, mTime);

		// Update scene
		updateCamera(_dt, uCamPos);

		// Keep window resolution updated
		Vec2u resolution = window().size();
		driver.setUniform(uResolution, Vec2f(float(resolution.x), float(resolution.y)));
		
		driver.drawIndexBuffer(6, indices, GraphicsDriver::EPrimitiveType::triangles);
		return true;
	}

	//----------------------------------------------------------------
	void updateCamera(float _dt, int _uCamPos) {
		if(mCam) {
			mCam->update(_dt);
			driver3d().setUniform(_uCamPos, mCam->position());
		}
	}
};

// ---- Generic main loop ----
int main(int _argc, const char** _argv) {
	
	ShaderToy app(_argc,_argv);

	while(app.update());
	return 0;
}