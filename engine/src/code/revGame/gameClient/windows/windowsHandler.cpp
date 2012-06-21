////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 20th, 2011
////////////////////////////////////////////////////////////////////////////////
// Windows handler: Deals with Windows OS

#ifdef WIN32
#include <Windows.h>

#include "windowsHandler.h"

#include <revInput/keyboardInput/windows/keyboardInputWindows.h>
#include <revInput/touchInput/windows/touchInputWin32.h>

using namespace rev::input;

namespace rev { namespace game
{
	//------------------------------------------------------------------------------------------------------------------
	CWindowsHandler::CWindowsHandler() 
		: mExitGameRequested(false)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	void CWindowsHandler::update()
	{
		MSG msg;
		CTouchInputWin32 * touch = static_cast<CTouchInputWin32*>(STouchInput::get());
		touch->refresh();

		while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message==WM_QUIT)
			{
				mExitGameRequested = true;
			}
			else
			{
                // Tell touch input to process OS messages
				CKeyboardInputWindows * keyboard = static_cast<CKeyboardInputWindows*>(SKeyboardInput::get());
				bool processed = keyboard->processWindowsMessage(msg);
				if (!processed)
				{
					processed = touch->processWindowsMessage(msg);
				}
				if(!processed)
				{
					TranslateMessage(&msg);	// Translate The Message
					DispatchMessage(&msg);
				}
			}
		}
	}
	
}	// namespace game
}	// namespace rev

#endif WIN32