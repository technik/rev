////////////////////////////////////////////////////////////////////////////////
// Revolution Engine game client
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 20th, 2011
////////////////////////////////////////////////////////////////////////////////
// Linux entry point
// Brief:
// This file contains the entry point and basic execution flow of the game
// client.
#ifdef _linux

// Standard headers
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

// Engine headers
#include <revCore/memory/memory.h>
#include <revCore/codeTools/log/log.h>
#include <revGame/gameClient/gameClient.h>
#include <revVideo/videoDriver/opengl/linux/videoDriverLinux.h>
#include <revVideo/video.h>

#include <rtl/vector.h>

using namespace rev::codeTools;
using namespace rev::game;
using namespace rev::rtl;
using namespace rev::video;


int main (int /*_argc*/, const char** /*_argv*/)
{
	// Create a basic game clinet.
	CGameClient	* pGame = new CGameClient();
	// Init the game client.
	pGame->init();

	// Get the video driver
	CVideoDriverLinux * videoDriver = reinterpret_cast<CVideoDriverLinux*>(SVideo::get()->driver());
	bool bExit = false; // Flag to tell whether the game has requested exit.
	while(!bExit)		// Keep the loop till the game requests exit.
	{
		XEvent event;
		while(videoDriver->nextXEvent(event))
		{
			switch(event.type)
			{
				// Exit if someone presses ESC
				case KeyPress:
				{
					KeySym     keysym;
					XKeyEvent *kEvent;
					char       buffer[1];
					/* It is necessary to convert the keycode to a
					* keysym before checking if it is an escape */
					kEvent = (XKeyEvent *) &event;
					if ((XLookupString(kEvent,buffer,1,&keysym,NULL) == 1)
						&& (keysym == (KeySym)XK_Escape) )
						bExit = true;
					break;
				}
			}
		}
		pGame->update();// Update the game client.
	}
	pGame->end();		// End the game client
	// Finally delete the game client
	delete pGame;
	return 0;
}

#endif // _linux
