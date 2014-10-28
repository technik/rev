//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
// on 2014-10-27
//----------------------------------------------------------------------------------------------------------------------
// Engine's hello world (simple triangle)

#include <core/memory/newAllocator.h>
#include <engine.h>

int main(int, const char**) {
	rev::Engine<rev::core::NewAllocator>	mEngine;
	mEngine::mainWindow();

	return 0;
}