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
#include "revGame/gameClient/gameClient.h"

using namespace rev::game;

int main (int /*_argc*/, const char ** /*_argv*/)
{
	CGameClient * gameClient = new CGameClient();
	gameClient->init();

	bool bExitGame = false;
	while(!bExitGame)
	{
		gameClient->update();
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
				TranslateMessage(&msg);	// Translate The Message
				DispatchMessage(&msg);
			}
		}
		//if(SInput::get()->keyboard()->pressed(0x1B))
		//	bExitGame = true;
	}

	gameClient->end();
	delete gameClient;
	return 0;
}

#endif // WIN32
