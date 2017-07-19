//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
// Engine's default player's entry point
#ifndef ANDROID
#include "player.h"
#include <core/platform/platformInfo.h>

int main(int _argc, const char** _argv) {
	rev::core::StartUpInfo pi{ _argc, _argv };
	rev::Player app(pi);

	while(app.update());
	return 0;
}

#endif // !ANDROID