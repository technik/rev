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
		enum class Key : uint32_t
		{
			BackSpace = VK_BACK,
			Tab,
			Enter = VK_RETURN,
			Shift = VK_SHIFT,
			Control,
			Alt,
			Pause,
			CapsLock,
			Escape = VK_ESCAPE,
			Space = VK_SPACE,
			PageUp,
			PageDown,
			End,
			Home,
			KeyLeft,
			KeyUp,
			KeyRight,
			KeyDown,
			Delete = VK_DELETE,
			A = 'A',
			B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
			Key0 = VK_NUMPAD0,
			Key1, Key2, Key3, Key4, Key5, Key6, Key7, Key8, Key9, Key,
			MaxKeyCode
		};
	public:
		KeyboardInputWindows();
		~KeyboardInputWindows();

		bool pressed	(Key _key) const;
		bool held		(Key _key) const;
		bool released	(Key _key) const;

		void refresh	();
		bool processWin32Message(MSG);

	private:
		static constexpr uint32_t MaxKeys = (uint32_t)Key::MaxKeyCode;
		int keyState[MaxKeys];
		int oldKeyState[MaxKeys];
	};

	typedef KeyboardInputWindows	KeyboardInputBase;

}	// namespace input
}	// namespace rev

#endif // _REV_INPUT_KEYBOARD_WINDOWS_KEYBOARDINPUTWINDOWS_H_
#endif // _WIN32