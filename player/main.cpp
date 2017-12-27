//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
// Engine's default player's entry point
#ifndef ANDROID
#include "player.h"

int main(int _argc, const char** _argv) {
	rev::Player app;

	for(;;) {
		app.render();
	}
	return 0;
}

#endif // !ANDROID