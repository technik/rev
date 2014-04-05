//----------------------------------------------------------------------------------------------------------------------
// Platformer game for testing purposes
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/March/30
//----------------------------------------------------------------------------------------------------------------------

#include <core/memory/newAllocator.h>
#include <engine.h>

typedef rev::Engine<rev::core::NewAllocator>	Engine;

//----------------------------------------------------------------------------------------------------------------------
int main(int _argc, const char** _argv) {
	Engine engine(_argc, _argv);

	while (engine.update())
	{
	}
	return 0;
}