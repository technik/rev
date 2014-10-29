//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014-10-29
//----------------------------------------------------------------------------------------------------------------------
// Window for linux systems
#ifndef _REV_VIDEO_WINDOW_LINUX_WINDOWLINUX_H_
#define _REV_VIDEO_WINDOW_LINUX_WINDOWLINUX_H_

#ifdef __linux__

#include <math/algebra/vector.h>

namespace rev {
	namespace video {

		class WindowLinux {
		public:
			WindowLinux(const math::Vec2u& _pos, const math::Vec2u& _size, const char* _windowName);
			
		private:
			math::Vec2u mPosition;
			math::Vec2u mSize;
		};

		typedef WindowLinux	WindowBase;

	}	// namespace video
}	// namespace rev

#endif // __linux__

#endif // _REV_VIDEO_WINDOW_LINUX_WINDOWLINUX_H_