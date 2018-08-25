//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
#include <core/platform/fileSystem/fileSystem.h>
#include <core/platform/osHandler.h>
#include <graphics/backend/device.h>
#include <graphics/backend/Windows/windowsPlatform.h>

//--------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------
int main(int _argc, const char** _argv) {
	// Init engine core systems
	rev::core::OSHandler::startUp();
	rev::core::FileSystem::init();

	// Create the application window
	auto wnd = rev::gfx::createWindow({100, 150}, { 200, 200 }, "Vulkraft", true);
	
	// Main loop
	for(;;)
	{
		if(!rev::core::OSHandler::get()->update())
			break;
	}

	// Clean up
	rev::core::FileSystem::end();
	rev::core::OSHandler::shutDown();
	return 0;
}
