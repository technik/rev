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

const float cStereoDistance = 0.1f;

// --- The triangle ---
const Vec3f vertices[] = {
	// Mono quad
	{-1.f, -1.f, 0.f },
	{1.f, -1.f, 0.f },
	{1.f, 1.f, 0.f },
	{-1.f, 1.f, 0.f },
	// Left quad
	{-1.f, -1.f, 0.f },
	{0.f, -1.f, 0.f },
	{0.f, 1.f, 0.f },
	{-1.f, 1.f, 0.f },
	// Right quad
	{0.f, -1.f, 0.f },
	{1.f, -1.f, 0.f },
	{1.f, 1.f, 0.f },
	{0.f, 1.f, 0.f },
};

const Vec2f uvs[] = {
	// Mono
	{0.f, 0.f},
	{1.f, 0.f},
	{1.f, 1.f},
	{0.f, 1.f},
	// Left
	{0.f, 0.f},
	{1.f, 0.f},
	{1.f, 1.f},
	{0.f, 1.f},
	// Right
	{0.f, 0.f},
	{1.f, 0.f},
	{1.f, 1.f},
	{0.f, 1.f},
};

uint16_t indicesMono[] = { 0, 1, 2, 2, 3, 0 };
uint16_t indicesLeft[] = { 4, 5, 6, 6, 7, 4 };
uint16_t indicesRight[] = { 8, 9, 10, 10, 11, 8 };

enum class CamMode {
	eMono,
	eLeft,
	eRight,
};

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
	bool			mStereo = false;
	
	//----------------------------------------------------------------
	void processArgs(int _argc, const char** _argv) {
		for(int i = 0; i < _argc; ++i) {
			std::string arg(_argv[i]);
			if(arg.substr(0,8)=="-shader=") {
				mShader = Shader::manager()->get(arg.substr(8));
			} else if(arg == "-flyby") {
				mCam = new FlyByCamera(1.57079f, 1.333f, 0.001f, 1000.0f); // Actually, inside the shader we only care for camera's position
			} else if(arg == "-stereo3d") {
				mStereo = true;
			}
		}
	}

	//----------------------------------------------------------------
	bool frame(float _dt) override {

		rev::video::GraphicsDriver& driver = driver3d();
		driver.setShader((Shader*)mShader);
		driver.setAttribBuffer(0, 4, vertices);
		driver.setAttribBuffer(2, 4, uvs);
		int uTime = driver.getUniformLocation("uTime");
		int uResolution = driver.getUniformLocation("uResolution");
		int uCamPos = driver.getUniformLocation("uCamPos");
		
		// Update time
		mTime += _dt;
		driver.setUniform(uTime, mTime);

		// Keep window resolution updated
		Vec2u resolution = window().size();
		if(mStereo)
			resolution.x /= 2;
		driver.setUniform(uResolution, Vec2f(float(resolution.x), float(resolution.y)));

		// Update scene
		updateCamera(_dt);
		if(mStereo) {
			// Draw left eye
			setupCamera(uCamPos, CamMode::eLeft);
			driver.drawIndexBuffer(6, indicesLeft, GraphicsDriver::EPrimitiveType::triangles);
			// Draw right eye
			setupCamera(uCamPos, CamMode::eRight);
			driver.drawIndexBuffer(6, indicesRight, GraphicsDriver::EPrimitiveType::triangles);
		} else {
			setupCamera(uCamPos, CamMode::eMono);
			driver.drawIndexBuffer(6, indicesMono, GraphicsDriver::EPrimitiveType::triangles);
		}
		return true;
	}

	//----------------------------------------------------------------
	void setupCamera(int _uCamPos, CamMode _mode) {
		Vec3f offset = Vec3f(0.0);
		if(_mode == CamMode::eLeft) {
			offset = Vec3f(-cStereoDistance,0.0,0.0);
		} else 
		if(_mode == CamMode::eRight) {
			offset = Vec3f(cStereoDistance,0.0,0.0);
		}
		// Send camera position to GPU
		if(mCam)
			driver3d().setUniform(_uCamPos, mCam->position()+offset);
		else // Default cam position
			driver3d().setUniform(_uCamPos, offset);
	}

	//----------------------------------------------------------------
	void updateCamera(float _dt) {
		if(mCam) {
			mCam->update(_dt);
		}
	}
};

// ---- Generic main loop ----
int main(int _argc, const char** _argv) {
	
	ShaderToy app(_argc,_argv);

	while(app.update());
	return 0;
}