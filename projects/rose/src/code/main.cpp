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

using namespace rev;
using namespace rev::graphics3d;

using rose::component::Avr8bit;
using namespace std;

class RoseApp : public Application3d
{
public:
	RoseApp()
	{
		obj = new Renderable();
		obj->m = rev::math::Mat34f::identity();
		obj->m[0][3] = 0.f;
		obj->m[1][3] = 5.f;
		obj->m[2][3] = 0.f;
	}

	~RoseApp()
	{
		delete obj;
	}

private:
	Renderable* obj;
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