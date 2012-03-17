////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on March 13th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Keyboard input for windows

#ifdef WIN32

#include "keyboardInputWindows.h"

namespace rev { namespace input
{
	//------------------------------------------------------------------------------------------------------------------
	CKeyboardInputWindows::CKeyboardInputWindows()
	{
		for (int key = 0; key < eMaxKeyCode; ++key)
		{
			keyState[key] = 0;
			oldKeyState[key] = 0;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	CKeyboardInputWindows::~CKeyboardInputWindows()
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	bool CKeyboardInputWindows::pressed(EKeyCode _key) const
	{
		return keyState[_key] == 1 && oldKeyState[_key] == 0;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool CKeyboardInputWindows::held(EKeyCode _key) const
	{
		return keyState[_key] == 1;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool CKeyboardInputWindows::released(EKeyCode _key) const
	{
		return keyState[_key] == 0;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CKeyboardInputWindows::refresh()
	{
		for (int key = 0; key < eMaxKeyCode; ++key)
		{
			oldKeyState[key] = keyState[key];
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	bool CKeyboardInputWindows::processWindowsMessage(MSG _msg)
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

#endif // WIN32