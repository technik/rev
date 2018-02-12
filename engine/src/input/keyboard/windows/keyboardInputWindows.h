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
#include <functional>
#include <vector>

namespace rev { namespace input
{
	class KeyboardInputWindows
	{
	public:
		enum class Key : uint8_t
		{
			BackSpace = VK_BACK,
			Tab = VK_TAB,
			Escape = VK_ESCAPE,
			Enter = VK_RETURN,
			Shift = VK_SHIFT,
			Control,
			Alt,
			Pause,
			CapsLock,
			PageUp = VK_PRIOR,
			PageDown,
			End,
			Home,
			KeyLeft,
			KeyUp,
			KeyRight,
			KeyDown,
			Delete = VK_DELETE,
			Key0 = VK_NUMPAD0,
			Key1, Key2, Key3, Key4, Key5, Key6, Key7, Key8, Key9, Key,
		};

	public:

		KeyboardInputWindows();
		~KeyboardInputWindows();

		bool pressed	(Key _key) const;
		bool held		(Key _key) const;
		bool released	(Key _key) const;

		using Callback = std::function<void(int)>;

		void onPress(Callback cb) { mOnPressEvents.push_back(cb); }
		void onRelease(Callback cb) { mOnReleaseEvents.push_back(cb); }

		void refresh	();
		bool processWin32Message(MSG);

	private:
		static constexpr uint32_t MaxKeys = 512;
		int keyState[MaxKeys];
		int oldKeyState[MaxKeys];
		std::vector<Callback>	mOnPressEvents;
		std::vector<Callback>	mOnReleaseEvents;
	};

	typedef KeyboardInputWindows	KeyboardInputBase;

}	// namespace input
}	// namespace rev

#endif // _REV_INPUT_KEYBOARD_WINDOWS_KEYBOARDINPUTWINDOWS_H_
#endif // _WIN32