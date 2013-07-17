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
#include <revGraphics3d/application/application3d.h>
#include <revGraphics3d/renderable/renderable.h>
#include <revGame/scene/camera/flyByCamera.h>

using namespace rev;
using namespace rev::graphics3d;

using rose::component::Avr8bit;
using namespace std;

class RoseApp : public Application3d
{
public:
	RoseApp()
	{
		obj = Renderable::geoSphere(1.f, 8, 5);
		obj->m = rev::math::Mat34f::identity();
		obj->m[0][3] = 0.f;
		obj->m[1][3] = 5.f;
		obj->m[2][3] = 0.f;
		obj2 = Renderable::box(math::Vec3f(2.f, 3.f, 4.f));
		obj2->m = rev::math::Mat34f::identity();
		obj2->m[0][3] = 3.f;
		obj2->m[1][3] = 6.f;
		obj2->m[2][3] = 1.f;

		cam = new game::FlyByCamera(1.6f, 1.333f, 0.125f, 10000.f);
		setCam(&cam->cam());
	}

	bool update() {
		cam->update();
		return true;
	}

	~RoseApp()
	{
		delete cam;
		delete obj;
		delete obj2;
	}

private:
	game::FlyByCamera* cam;
	Renderable* obj, *obj2;
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