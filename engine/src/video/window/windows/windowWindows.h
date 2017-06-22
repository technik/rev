//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Window for Win32 systems
#ifndef _REV_VIDEO_WINDOW_WINDOWS_WINDOWWINDOWS_H_
#define _REV_VIDEO_WINDOW_WINDOWS_WINDOWWINDOWS_H_

#ifdef _WIN32

#include <Windows.h>

#include <math/algebra/vector.h>

namespace rev {
	namespace video {
		
		class WindowWindows {
		public:
			WindowWindows(const math::Vec2u& _pos, const math::Vec2u& _size, const char* _windowName);
			inline HWND		winapiHandle() const { return mWinapiHandle; }

			const math::Vec2u& size() const { return mSize; }
			const math::Vec2u& pos() const { return mPosition; }

		private:
			math::Vec2u mPosition;
			math::Vec2u mSize;
			HWND	mWinapiHandle; // Winapi handle to the window

		private:
			virtual void invokeOnResize() const = 0;

			bool processWin32Message(MSG _msg);
			static bool sIsClassRegistered;
			static void registerClass();
		};

		typedef WindowWindows	WindowBase;

	}	// namespace video
}	// namespace rev

#endif // _WIN32

#endif // _REV_VIDEO_WINDOW_WINDOWS_WINDOWWINDOWS_H_