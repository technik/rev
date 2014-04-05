//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
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

		private:
			math::Vec2u mPosition;
			math::Vec2u mSize;
			HWND	mWinapiHandle; // Winapi handle to the window

		private:
			static bool sIsClassRegistered;
			static void registerClass();
		};

		typedef WindowWindows	WindowBase;

	}	// namespace video
}	// namespace rev

#endif // _WIN32

#endif // _REV_VIDEO_WINDOW_WINDOWS_WINDOWWINDOWS_H_