////////////////////////////////////////////////////////////////////////////////
// Revolution Engine game client
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 20th, 2011
////////////////////////////////////////////////////////////////////////////////
// Windows entry point
// Brief:
// This file contains the entry point and basic execution flow of the game
// client.
#ifdef WIN32

// Standard headers
#include <windows.h>

// Engine headers
#include <revInput/keyboardInput/keyboardInput.h>

// Project headers
#include <revGameClient.h>

using namespace rev::input;

int main (int /*_argc*/, const char ** /*_argv*/)
{
	// Create the game client
	rev::game::SGameClient::create();

	// Loop
	bool bExitGame = false;
	while(!bExitGame)
	{
		bExitGame = !rev::game::SGameClient::get()->update();
		// Access the mouse
		SKeyboardInput * keyboard = SKeyboardInput::get();
		bExitGame |= keyboard->pressed(SKeyboardInput::eEscape);
	}

	rev::game::SGameClient::destroy();
	return 0;
}

#endif // WIN32
