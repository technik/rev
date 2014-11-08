////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on March 13th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Keyboard input for windows

#ifdef _WIN32
#ifndef _REV_INPUT_KEYBOARD_WINDOWS_KEYBOARDINPUTWINDOWS_H_
#define _REV_INPUT_KEYBOARD_WINDOWS_KEYBOARDINPUTWINDOWS_H_

#include <Windows.h>

namespace rev { namespace input
{
	class KeyboardInputWindows
	{
	public:
		enum EKeyCode
		{
			eBackSpace = VK_BACK,
			eTab,
			eEnter = VK_RETURN,
			eShift = VK_SHIFT,
			eControl,
			eAlt,
			ePause,
			eCapsLock,
			eEscape = VK_ESCAPE,
			eSpace = VK_SPACE,
			ePageUp,
			ePageDown,
			eEnd,
			eHome,
			eKeyLeft,
			eKeyUp,
			eKeyRight,
			eKeyDown,
			eA = 'A',
			eB, eC, eD, eE, eF, eG, eH, eI, eJ, eK, eL, eM, eN, eO, eP, eQ, eR, eS, eT, eU, eV, eW, eX, eY, eZ,

			eMaxKeyCode
		};
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

	typedef KeyboardInputWindows	KeyboardInputBase;

}	// namespace input
}	// namespace rev

#endif // _REV_INPUT_KEYBOARD_WINDOWS_KEYBOARDINPUTWINDOWS_H_
#endif // _WIN32