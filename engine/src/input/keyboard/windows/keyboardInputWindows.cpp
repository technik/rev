////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on March 13th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Keyboard input for windows

#ifdef _WIN32

#include "keyboardInputWindows.h"

using namespace rev::core;

namespace rev { namespace input
{
	//------------------------------------------------------------------------------------------------------------------
	KeyboardInputWindows::KeyboardInputWindows()
	{
		for (int key = 0; key < MaxKeys; ++key)
		{
			keyState[key] = 0;
			oldKeyState[key] = 0;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	KeyboardInputWindows::~KeyboardInputWindows()
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	bool KeyboardInputWindows::pressed(Key _key) const
	{
		return keyState[(unsigned)_key] == 1 && oldKeyState[(unsigned)_key] == 0;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool KeyboardInputWindows::held(Key _key) const
	{
		return keyState[(unsigned)_key] == 1;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool KeyboardInputWindows::released(Key _key) const
	{
		return keyState[(unsigned)_key] == 0;
	}

	//------------------------------------------------------------------------------------------------------------------
	void KeyboardInputWindows::refresh()
	{
		// Save old state
		for (int key = 0; key < MaxKeys; ++key)
		{
			oldKeyState[key] = keyState[key];
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	bool KeyboardInputWindows::processWin32Message(MSG msg) {
		if (msg.message == WM_KEYDOWN)
		{
			if (msg.wParam < MaxKeys)
			{
				keyState[msg.wParam] = 1;
				for(auto& cb : mOnPressEvents)
					cb(Key(msg.wParam));
				return true;
			}
		}
		else if (msg.message == WM_KEYUP)
		{
			if (msg.wParam < MaxKeys)
			{
				keyState[msg.wParam] = 0;
				for(auto& cb : mOnReleaseEvents)
					cb(Key(msg.wParam));
				return true;
			}
		}
		return false;
	}


}	// namespace input
}	// namespace rev

#endif // _WIN32