////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Buggy Demo
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// On November 26th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
// Standard header files
#include <Windows.h>

// Engine headers
#include <revInput/touchInput/windows/touchInputWin32.h>

// Project specific headers
#include "gameclient.h"

int main (int /*_argc*/, const char ** /*_argv*/)
{
	CBuggyGameClient * gameClient = new CBuggyGameClient();

	bool bExitGame = false;
	while(!bExitGame)
	{
		// Access the mouse
		rev::input::CTouchInputWin32 * touchInputSystem = static_cast<rev::input::CTouchInputWin32*>(rev::input::STouchInput::get());
		touchInputSystem->refresh();
		// This prevents the application from hanging and makes it responsive to Windows events
		MSG msg;
		while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message==WM_QUIT)
			{
				bExitGame = true;
			}
			else
			{
                // Tell touch input to process OS messages
                touchInputSystem->processWindowsMessage(msg);
				TranslateMessage(&msg);	// Translate The Message
				DispatchMessage(&msg);
			}
		}
		// Update the game client
		bExitGame = bExitGame || !gameClient->update();
	}

	delete gameClient;
	return 0;
}

#endif // WIN32