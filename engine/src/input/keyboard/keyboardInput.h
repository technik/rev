////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on March 13th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Keyboard input

#ifndef _REV_INPUT_KEYBOARD_KEYBOARDINPUT_H_
#define _REV_INPUT_KEYBOARD_KEYBOARDINPUT_H_

#ifdef WIN32
#include "windows/keyboardInputWindows.h"
#endif // WIN32
#ifdef __linux__
#endif // __linux__

namespace rev { namespace input
{
	class KeyboardInput : public KeyboardInputBase
	{
	public:

	public:
		static void				init();
		static void				end();
		static KeyboardInput *	get() {
			return sInstance;
		}

	private:
		static KeyboardInput * sInstance;
	};
}	// namespace input
}	// namespace input

#endif // _REV_INPUT_KEYBOARD_KEYBOARDINPUT_H_