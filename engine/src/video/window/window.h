//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Window interface
#ifndef _REV_VIDEO_WINDOW_WINDOW_H_
#define _REV_VIDEO_WINDOW_WINDOW_H_

#ifdef _WIN32
#include "windows/windowWindows.h"
#endif // _WIN32
#ifdef __linux__
#include "linux/windowLinux.h"
#endif //__linux__

namespace rev {
	namespace video {

		class Window : public WindowBase {
		public:
			Window(const math::Vec2u& _pos, const math::Vec2u& _size, const char* _windowName)
				:WindowBase(_pos, _size, _windowName)
			{}
		};

	}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_WINDOW_WINDOW_H_