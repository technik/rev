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
	#ifdef ANDROID
		#include "windowAndroid.h"
	#else // !ANDROID
		#include "linux/windowLinux.h"
	#endif // !ANDROID
#endif //__linux__

#include <cassert>
#include <core/event.h>
#include <math/algebra/vector.h>
#include <video/graphics/driver/graphicsDriver.h>
#include <video/graphics/driver/NativeFrameBuffer.h>

namespace rev {
	namespace video {

		class Window : public WindowBase {
		public:
			Window(const math::Vec2u& _pos, const math::Vec2u& _size, const char* _windowName)
				:WindowBase(_pos, _size, _windowName)
			{}

			NativeFrameBuffer& frameBuffer() const { 
				// If the window exists, and the driver exists, then it must have been created with a native frame buffer
				assert(GraphicsDriver::get().nativeFrameBuffer());
				return *GraphicsDriver::get().nativeFrameBuffer(); 
			}

			typedef rev::core::Event<>	ResizeEvent;
			ResizeEvent& onResize() { return mOnResize; }

		private:
			ResizeEvent	mOnResize;
			void invokeOnResize() const { mOnResize(); }
		};

	}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_WINDOW_WINDOW_H_