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
	CBuggyGameClient * client = new CBuggyGameClient();

	bool bExitGame = false;
	while(!bExitGame)
	{
		bExitGame = !client->update();
		// Access the mouse
		SKeyboardInput * keyboard = SKeyboardInput::get();
		bExitGame |= keyboard->pressed(SKeyboardInput::eEscape);
	}

	delete client;
	return 0;
}