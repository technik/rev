//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
// A doom 3 map loader
#include <core/platform/fileSystem/fileSystem.h>
#include <core/platform/osHandler.h>
#include <graphics/backend/commandBuffer.h>
#include <graphics/backend/device.h>
#include <graphics/backend/Windows/windowsPlatform.h>
#include <graphics/backend/OpenGL/deviceOpenGLWindows.h>
#include <graphics/scene/renderGeom.h>

#include <string>
#include <vector>

using namespace std;
using namespace rev::core;
using namespace rev::gfx;
using namespace rev::math;

//--------------------------------------------------------------------------------------------------------------
int main(int _argc, const char** _argv) {
	// Init engine core systems
	OSHandler::startUp();
	FileSystem::init();

	// Parse arguments
	vector<string> arguments(_argc);
	for(int i = 0; i < _argc; ++i)
		arguments[i] = _argv[i];
	string levelName;
	for(int i = 0; i < _argc; ++i)
	{
		if(arguments[i] == "-i" && (i+1)<_argc)
			levelName = arguments[++i];
	}

	if(levelName.empty())
		return -1;

	// Clean up
	FileSystem::end();
	return 0;
}
