////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fern�ndez-Ag�era Tortosa (a.k.a. Technik)
// Created on March 13th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Keyboard input for windows

#ifdef WIN32
#ifndef _REV_INPUT_KEYBOARD_WINDOWS_KEYBOARDINPUTWINDOWS_H_
#define _REV_INPUT_KEYBOARD_WINDOWS_KEYBOARDINPUTWINDOWS_H_

#include <Windows.h>

#include <revInput/keyboard/keyboardInput.h>

namespace rev { namespace input
{
	class KeyboardInputWindows: public KeyboardInput
	{
	public:
		KeyboardInputWindows();
		~KeyboardInputWindows();

		bool pressed	(EKeyCode _key) const;
		bool held		(EKeyCode _key) const;
		bool released	(EKeyCode _key) const;

		void refresh	();

		bool processWindowsMessage(MSG _msg);	// Returns true if the message was processed.

	private:
		int keyState[eMaxKeyCode];
		int oldKeyState[eMaxKeyCode];
	};
}	// namespace input
}	// namespace rev

#endif // _REV_INPUT_KEYBOARD_WINDOWS_KEYBOARDINPUTWINDOWS_H_
#endif // WIN32