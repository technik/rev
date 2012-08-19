//----------------------------------------------------------------------------------------------------------------------
// Rose: Robot oriented simulation environment
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 8th, 2012
//----------------------------------------------------------------------------------------------------------------------
// main, entry point


#include <Windows.h>

#include <driver3d/driver3d.h>
#include <videoDriver/videoDriver.h>

using namespace rev;

//----------------------------------------------------------------------------------------------------------------------
int main ( int _argc, const char ** _argv )
{
	_argc; _argv;// Unused arguments




	video::VideoDriver::startUp();
	video::VideoDriver::get()->init3d();
	video::VideoDriver::getDriver3d()->finishFrame();
	system("PAUSE");


	// Simulation workflow

	// Can either do a simulation, edit agents or environments
	// or just visualize some data

	// Get simulation definition (information needed to create a simulation)
	// Simulate while storing data
	// Display results

	return 0;
}