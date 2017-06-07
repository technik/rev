//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014-10-29
//----------------------------------------------------------------------------------------------------------------------
// Window for linux systems
#ifndef _REV_VIDEO_WINDOW_LINUX_WINDOWLINUX_H_
#define _REV_VIDEO_WINDOW_LINUX_WINDOWLINUX_H_

#if defined( __linux__ ) && !defined(ANDROID)

#include <math/algebra/vector.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/glew.h>
#include <GL/glx.h>

namespace rev {
	namespace video {

		class WindowLinux {
		public:
			WindowLinux(const math::Vec2u& _pos, const math::Vec2u& _size, const char* _windowName);
			
			const math::Vec2u& size() const { return mSize; }
			const math::Vec2u& pos() const { return mPosition; }
			
			Display* display() const { return mDisplay; }
			::Window xWindow() const { return mXWindow; }
			XVisualInfo* visualInfo() const { return mVisualInfo; }
		private:
			math::Vec2u mPosition;
			math::Vec2u mSize;

			Display* mDisplay = nullptr;
			::Window mXWindow;
			XVisualInfo* mVisualInfo = nullptr;
		};

		typedef WindowLinux	WindowBase;

	}	// namespace video
}	// namespace rev

#endif // __linux__ && !ANDROID

#endif // _REV_VIDEO_WINDOW_LINUX_WINDOWLINUX_H_