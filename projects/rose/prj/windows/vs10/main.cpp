//----------------------------------------------------------------------------------------------------------------------
// Rose: Robot oriented simulation environment
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 8th, 2012
//----------------------------------------------------------------------------------------------------------------------
// main, entry point

#include <videoDriver/videoDriver.h>

#include <Windows.h>
#include <driver3d/openGL20/windows/driver3dOpenGL20Windows.h>

using namespace rev;

//----------------------------------------------------------------------------------------------------------------------
int main ( int _argc, const char ** _argv )
{
	_argc; _argv;// Unused arguments




	video::VideoDriver::startUp();
	video::Window * window = video::VideoDriver::get()->createWindow(math::Vec2i(50, 50), math::Vec2u(320, 180));

	rev::video::Driver3dOpenGL20Windows * driver = new rev::video::Driver3dOpenGL20Windows(window);
	driver->swapBuffers();
	system("PAUSE");


	// Simulation workflow

	// Can either do a simulation, edit agents or environments
	// or just visualize some data

	// Get simulation definition (information needed to create a simulation)
	// Simulate while storing data
	// Display results

	return 0;
}