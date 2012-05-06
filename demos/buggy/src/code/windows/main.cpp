////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Buggy Demo
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// On November 26th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Engine headers
#include <revInput/keyboardInput/keyboardInput.h>

// Project specific headers
#include <gameclient.h>

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