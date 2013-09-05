//----------------------------------------------------------------------------------------------------------------------
// Rose project
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On October 8th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Entry point

#include <iostream>
#include <string>

#include <aircraft/quad/quad.h>
#include <component/microcontroller/avr/avr8bit.h>
#include <revCore/codeTools/log/log.h>
#include <revCore/time/time.h>
#include <revGame/application/baseApplication.h>
#include <revGame/core/transform/sceneNode.h>
#include <revGame/physics/rigidBody/rigidBody.h>
#include <revGame/physics/world/physicsWorld.h>
#include <revGame/scene/camera/flyByCamera.h>
#include <revGame/scene/level/doom3Level.h>
#include <revGame/scene/object/solidObject.h>
#include <revGraphics3d/renderer/renderer.h>
#include <revGraphics3d/renderer/forward/forwardRenderer.h>
#include <revInput/keyboard/keyboardInput.h>
#include <revPlatform/fileSystem/fileSystem.h>
#include <revVideo/driver3d/driver3d.h>
#include <revVideo/driver3d/openGL21/driver3dOpenGL21.h>
#include <revVideo/types/color/color.h>
#include <revVideo/videoDriver/videoDriver.h>

using namespace rev;
using namespace rev::math;
using namespace rev::game;
using namespace rev::graphics3d;
using namespace rev::input;
using namespace rev::platform;

using rose::component::Avr8bit;
using namespace std;

class RoseApp : public BaseApplication
{
public:
	RoseApp(int _argc, const char** _argv)
		:mCam(nullptr)
		,mLevel(nullptr)
	{
		video::VideoDriver::getDriver3d()->setClearColor(video::Color(0.f));
		processArguments(_argc, _argv);
		if(!mMapName.empty()) {
		// --- Doom level loading ---
		mLevel = new Doom3Level(mMapName.c_str());

		mCam = new game::FlyByCamera(1.0f, 1.333f, 0.125f, 10000.f);
		setCam(&mCam->cam());
		mCam->node()->setPos(mLevel->playerStart + Vec3f::zAxis() * 10);

		mWorld = PhysicsWorld::get();
		mWorld->setGravity(9.81f);
		}
		else { // Raytracing
			Vec3f volume(600.f, 100.f, 800.f);
			mRayCam = new graphics3d::Camera(Mat44f::ortho(volume));
			setCam(mRayCam); // Hack, not necessary for transform, but renderer will skip if no camera registered
			mRayCanvas = graphics3d::Renderable::plane(Vec2f(2.f, 2.f));
			mRayCanvas->m = Mat34f::identity();

			auto loadRaytraceShader = [](const char*) {
				video::Driver3dOpenGL21* glDriver = static_cast<video::Driver3dOpenGL21*>(video::VideoDriver::getDriver3d());
				unsigned program = glDriver->loadShader("raytrace.vtx", "raytrace.pxl");
				glDriver->setShader(program);
			};
			loadRaytraceShader(nullptr);
			platform::FileSystem::get()->onFileChanged("raytrace.vtx") += loadRaytraceShader;
			platform::FileSystem::get()->onFileChanged("raytrace.pxl") += loadRaytraceShader;
		}
	}

	void processArguments(int _argc, const char** _argv){
		for(auto i = 0; i < _argc; ++i){
			if(!strncmp(_argv[i], "--map", 5)) {
				mMapName = _argv[++i];
				continue;
			}
		}
	}

	void renderScene(const rev::graphics3d::Camera* _camera)
	{
		if(mLevel)
			mRenderer->render(*_camera, *mLevel);
		else {
			// --- Render raytraced scene ---
			mRayCanvas->render();
		}
	}

	bool update() {
		if(!mLevel)
			return true;
		mCam->update();
		float deltaTime = Time::get()->frameTime();
		// --- Gameplay ---

		KeyboardInput* input = KeyboardInput::get();
		if(input->pressed(KeyboardInput::eP))
			mLevel->mFilterBsp = !mLevel->mFilterBsp;
		if(input->pressed(KeyboardInput::eL))
			static_cast<ForwardRenderer*>(mRenderer)->swapLock();

		// ----------------
		mWorld->update(deltaTime);
		return true;
	}

	~RoseApp()
	{
		if(mCam) delete mCam;
	}

private:
	// Doom
	FlyByCamera*	mCam;
	SolidObject*	floor;
	SolidObject*	ball;
	Quad*			mQuadcopter;
	PhysicsWorld*	mWorld;
	Doom3Level*		mLevel;
	std::string		mMapName;

	// Raytrace
	graphics3d::Camera*	mRayCam;
	graphics3d::Renderable* mRayCanvas;
};

int main(int _argc, const char** _argv)
{
	// System initialization
	codeTools::Log::init();
	revLog() << "Rose: Robotic Simulation Environment\n";

	RoseApp app(_argc, _argv);
	app.run();

	// House keeping
	codeTools::Log::end();
	return 0;
}