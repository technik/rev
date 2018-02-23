////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on 2014-10-29
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Keyboard input for linux

#ifdef __linux__
#ifndef _REV_INPUT_KEYBOARD_LINUX_KEYBOARDINPUTLINUX_H_
#define _REV_INPUT_KEYBOARD_LINUX_KEYBOARDINPUTLINUX_H_

namespace rev { namespace input
{
	class KeyboardInputLinux
	{
	public:
		enum class Key : uint8_t
		{
			BackSpace,
			Tab,
			Enter,
			Shift,
			Control,
			Alt,
			Pause,
			CapsLock,
			Escape,
			Space,
			PageUp,
			PageDown,
			End,
			Home,
			KeyLeft,
			KeyUp,
			KeyRight,
			KeyDown
		};
	public:
		KeyboardInputLinux() {}
		~KeyboardInputLinux() {}

		bool pressed	(Key _key) const { return false; }
		bool held		(Key _key) const { return false; }
		bool released	(Key _key) const { return false; } 

		bool pressed	(uint8_t _key) const { return pressed((Key)_key); }
		bool held		(uint8_t _key) const { return held((Key)_key); }
		bool released	(uint8_t _key) const { return released((Key)_key); }

		void refresh	() {}
		using Callback = std::function<void(int)>;

		void onPress(Callback cb) { }
		void onRelease(Callback cb) { }

	private:
	};

	typedef KeyboardInputLinux	KeyboardInputBase;

}	// namespace input
}	// namespace rev

#endif // _REV_INPUT_KEYBOARD_LINUX_KEYBOARDINPUTLINUX_H_
#endif // __linux__