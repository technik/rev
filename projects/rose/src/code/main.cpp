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
#include <revVideo/driver3d/driver3d.h>
#include <revVideo/types/color/color.h>
#include <revVideo/videoDriver/videoDriver.h>

using namespace rev;
using namespace rev::math;
using namespace rev::game;
using namespace rev::graphics3d;
using namespace rev::input;

using rose::component::Avr8bit;
using namespace std;

class RoseApp : public BaseApplication
{
public:
	RoseApp()
	{
		video::VideoDriver::getDriver3d()->setClearColor(video::Color(0.f));
		
		mLevel = new Doom3Level("d3dm3");

		cam = new game::FlyByCamera(1.0f, 1.333f, 0.125f, 10000.f);
		setCam(&cam->cam());
		cam->node()->setPos(mLevel->playerStart + Vec3f::zAxis() * 10);

		mWorld = PhysicsWorld::get();
		mWorld->setGravity(9.81f);

		// floor = SolidObject::box(0.f, Vec3f(10000.f, 10000.f, 1.f));
		// floor->rigidBody()->setPosition(Vec3f(0.f, 0.f, -0.5f));

		// ball = SolidObject::ball(0.f, 2.f, 10);
		// ball->rigidBody()->setPosition(Vec3f(4.f, 2.f, 1.f));

		// mQuadcopter = new Quad(0.7f, 2.f);
	}

	void renderScene(const rev::graphics3d::Camera* _camera)
	{
		mRenderer->render(*_camera, *mLevel);
	}

	bool update() {
		cam->update();
		float deltaTime = Time::get()->frameTime();
		// --- Gameplay ---

		KeyboardInput* input = KeyboardInput::get();
		if(input->pressed(KeyboardInput::eP))
			mLevel->mMaxRenderables++;
		if(input->pressed(KeyboardInput::eL) && mLevel->mMaxRenderables)
			static_cast<ForwardRenderer*>(mRenderer)->swapLock();
		//mQuadcopter->update(deltaTime);


		// ----------------
		mWorld->update(deltaTime);
		return true;
	}

	~RoseApp()
	{
		delete cam;
	}

private:
	FlyByCamera*	cam;
	SolidObject*	floor;
	SolidObject*	ball;
	Quad*			mQuadcopter;
	PhysicsWorld*	mWorld;
	Doom3Level*		mLevel;
};

int main()
{
	// System initialization
	codeTools::Log::init();
	revLog() << "Rose: Robotic Simulation Environment\n";

	RoseApp app;
	app.run();

	// House keeping
	codeTools::Log::end();
	return 0;
}