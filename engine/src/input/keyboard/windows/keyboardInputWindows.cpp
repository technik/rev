////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on March 13th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Keyboard input for windows

#ifdef _WIN32

#include "keyboardInputWindows.h"
#include <core/platform/osHandler.h>

using namespace rev::core;

namespace rev { namespace input
{
	//------------------------------------------------------------------------------------------------------------------
	KeyboardInputWindows::KeyboardInputWindows()
	{
		for (int key = 0; key < eMaxKeyCode; ++key)
		{
			keyState[key] = 0;
			oldKeyState[key] = 0;
		}

		// Register in osHandler for message processing
		(*OSHandler::get()) += [this](MSG _msg){
			return processWindowsMessage(_msg);
		};
	}

	//------------------------------------------------------------------------------------------------------------------
	KeyboardInputWindows::~KeyboardInputWindows()
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	bool KeyboardInputWindows::pressed(EKeyCode _key) const
	{
		return keyState[_key] == 1 && oldKeyState[_key] == 0;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool KeyboardInputWindows::held(EKeyCode _key) const
	{
		return keyState[_key] == 1;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool KeyboardInputWindows::released(EKeyCode _key) const
	{
		return keyState[_key] == 0;
	}

	//------------------------------------------------------------------------------------------------------------------
	void KeyboardInputWindows::refresh()
	{
		for (int key = 0; key < eMaxKeyCode; ++key)
		{
			oldKeyState[key] = keyState[key];
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	bool KeyboardInputWindows::processWindowsMessage(MSG _msg)
	{
		if(_msg.message == WM_KEYDOWN)
		{
			if(_msg.wParam < eMaxKeyCode)
			{
				keyState[_msg.wParam] = 1;
				return true;
			}
		}
		else if(_msg.message == WM_KEYUP)
		{
			if(_msg.wParam < eMaxKeyCode)
			{
				keyState[_msg.wParam] = 0;
				return true;
			}
		}
		return false;
	}

}	// namespace input
}	// namespace rev

#endif // _WIN32