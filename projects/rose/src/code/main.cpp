//----------------------------------------------------------------------------------------------------------------------
// Rose project
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On October 8th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Entry point

#include <iostream>
#include <string>

#include <component/microcontroller/avr/avr8bit.h>
#include <revCore/codeTools/log/log.h>
#include <revCore/time/time.h>
#include <revGraphics3d/application/application3d.h>
#include <revGame/physics/rigidBody/rigidBody.h>
#include <revGame/physics/world/physicsWorld.h>
#include <revGame/scene/camera/flyByCamera.h>
#include <revGame/scene/object/solidObject.h>

using namespace rev;
using namespace rev::math;
using namespace rev::game;
using namespace rev::graphics3d;

using rose::component::Avr8bit;
using namespace std;

class RoseApp : public Application3d
{
public:
	RoseApp()
	{
		cam = new game::FlyByCamera(1.0f, 1.333f, 0.125f, 10000.f);
		setCam(&cam->cam());

		mWorld = PhysicsWorld::get();
		mWorld->setGravity(9.81f);

		floor = SolidObject::box(0.f, Vec3f(100.f, 100.f, 1.f));
		floor->rigidBody()->setPosition(Vec3f(0.f, 10.f, -0.5f));

		ball = SolidObject::ball(10.f, 1.f, 16);
		ball->rigidBody()->setPosition(Vec3f(0.f, 10.f, 2.f));
	}

	bool update() {
		cam->update();
		float deltaTime = Time::get()->frameTime();
		mWorld->update(deltaTime);
		return true;
	}

	~RoseApp()
	{
		delete cam;
	}

private:
	FlyByCamera* cam;
	SolidObject* floor;
	SolidObject* ball;
	PhysicsWorld* mWorld;
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