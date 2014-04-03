//----------------------------------------------------------------------------------------------------------------------
// Platformer game for testing purposes
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/March/30
//----------------------------------------------------------------------------------------------------------------------

#include <core/memory/stdAllocator.h>
#include <engine.h>

typedef rev::Engine<rev::core::StdAllocator>	Engine;

//----------------------------------------------------------------------------------------------------------------------
int main(int _argc, const char** _argv) {
	Engine engine(_argc, _argv);

	return 0;
}