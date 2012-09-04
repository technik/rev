//----------------------------------------------------------------------------------------------------------------------
// Rose: Robot oriented simulation environment
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 8th, 2012
//----------------------------------------------------------------------------------------------------------------------
// main, entry point


#include <Windows.h>

#include <driver3d/driver3d.h>
#include <types/shader/pixel/pxlShader.h>
#include <types/shader/shader.h>
#include <types/shader/vertex/vtxShader.h>
#include <videoDriver/videoDriver.h>

using namespace rev;

//----------------------------------------------------------------------------------------------------------------------
int main ( int _argc, const char ** _argv )
{
	_argc; _argv;// Unused arguments




	video::VideoDriver::startUp();
	video::VideoDriver::get()->init3d();
	video::VideoDriver::getDriver3d()->finishFrame();

	video::VtxShader * vtx = video::VtxShader::get("test.vtx");
	video::PxlShader * pxl = video::PxlShader::get("test.pxl");
	video::Shader::get(std::make_pair(vtx,pxl));
	system("PAUSE");


	// Simulation workflow

	// Can either do a simulation, edit agents or environments
	// or just visualize some data

	// Get simulation definition (information needed to create a simulation)
	// Simulate while storing data
	// Display results

	return 0;
}