//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Window interface
#ifndef _REV_VIDEO_WINDOW_WINDOW_INL_
#define _REV_VIDEO_WINDOW_WINDOW_INL_

#include "window.h"
#include <math/algebra/vector.h>

#ifdef _WIN32
#include "windows/windowWindows.h"
#endif // _WIN32

namespace rev {
	namespace video {

		//--------------------------------------------------------------------------------------------------------------
		template<class Allocator_>
		Window* Window::createWindow(Allocator_& _alloc) {
#ifdef _WIN32
			return _alloc.create<WindowWindows>(math::Vec2u(100, 100), math::Vec2u(320, 240), "RevWindows");
#endif // _WIN32
		}

	}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_WINDOW_WINDOW_INL_