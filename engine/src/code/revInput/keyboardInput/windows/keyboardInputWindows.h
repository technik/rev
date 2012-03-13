////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on March 13th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Keyboard input for windows

#ifdef WIN32
#ifndef _REV_INPUT_KEYBOARDINPUT_WINDOWS_KEYBOARDINPUTWINDOWS_H_
#define _REV_INPUT_KEYBOARDINPUT_WINDOWS_KEYBOARDINPUTWINDOWS_H_

#include <Windows.h>

#include <revInput/keyboardInput/keyboardInput.h>

namespace rev { namespace input
{
	class CKeyboardInputWindows: public SKeyboardInput
	{
	public:
		CKeyboardInputWindows();
		~CKeyboardInputWindows();

		bool pressed	(EKeyCode _key) const;
		bool held		(EKeyCode _key) const;
		bool released	(EKeyCode _key) const;

		bool processWindowsMessage(MSG _msg);	// Returns true if the message was processed.

	private:
		int keyState[eMaxKeyCode];
	};
}	// namespace input
}	// namespace rev

#endif // _REV_INPUT_KEYBOARDINPUT_WINDOWS_KEYBOARDINPUTWINDOWS_H_
#endif // WIN32