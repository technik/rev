//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
// on 2014-10-27
//----------------------------------------------------------------------------------------------------------------------
// Engine's hello world (simple triangle)

#include <core/memory/newAllocator.h>
#include <engine.h>

int main(int _argc, const char** _argv) {
	rev::Engine<rev::core::NewAllocator>	mEngine(_argc,_argv);
	mEngine.mainWindow();

	return 0;
}