////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on March 13th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Keyboard input

#ifndef _REV_INPUT_KEYBOARD_KEYBOARDINPUT_H_
#define _REV_INPUT_KEYBOARD_KEYBOARDINPUT_H_

#ifdef _WIN32
#include "windows/keyboardInputWindows.h"
#endif // _WIN32
#ifdef __linux__
#include "linux/keyboardInputLinux.h"
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
#ifdef ANDROID
			return nullptr;
#else
			return sInstance;
#endif
		}

	private:
		static KeyboardInput * sInstance;
	};
}	// namespace input
}	// namespace input

#endif // _REV_INPUT_KEYBOARD_KEYBOARDINPUT_H_