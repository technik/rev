//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Window interface
#ifndef _REV_VIDEO_WINDOW_WINDOW_H_
#define _REV_VIDEO_WINDOW_WINDOW_H_

namespace rev {
	namespace video {

		class Window {
		public:
			virtual ~Window				() = default;
			template<class Allocator_>
			static Window* createWindow	(Allocator_&);
		protected:
			Window() = default;
		};

	}	// namespace video
}	// namespace rev

#include "window.inl"

#endif // _REV_VIDEO_WINDOW_WINDOW_H_