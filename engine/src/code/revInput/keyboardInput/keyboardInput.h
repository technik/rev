////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on March 13th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Keyboard input

#ifndef _REV_INPUT_KEYBOARDINPUT_KEYBOARDINPUT_H_
#define _REV_INPUT_KEYBOARDINPUT_KEYBOARDINPUT_H_

#ifdef WIN32
#include <Windows.h>
#endif // WIN32

namespace rev { namespace input
{
	class SKeyboardInput
	{
	public:
		enum EKeyCode
		{
#ifdef WIN32
			eKeyLeft = VK_LEFT,
#endif // WIN32
			eKeyUp,
			eKeyRight,
			eKeyDown,

			eMaxKeyCode
		};

	public:
		static void				init();
		static void				end();
		static SKeyboardInput *	get();

		virtual bool pressed	(EKeyCode _key) const = 0;
		virtual bool held		(EKeyCode _key) const = 0;
		virtual bool released	(EKeyCode _key) const = 0;

	protected:
		SKeyboardInput() {}
		virtual ~SKeyboardInput() {}

	private:
		static SKeyboardInput * sInstance;
	};
}	// namespace input
}	// namespace input

#endif // _REV_INPUT_KEYBOARDINPUT_KEYBOARDINPUT_H_