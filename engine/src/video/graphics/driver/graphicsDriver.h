//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/05
//----------------------------------------------------------------------------------------------------------------------
// Interface with graphics hardware

#ifndef _REV_VIDEO_GRAPHICS_DRIVER_GRAPHICSDRIVER_H_
#define _REV_VIDEO_GRAPHICS_DRIVER_GRAPHICSDRIVER_H_

#include "openGL/android/openGLDriverAndroid.h"

namespace rev {
	namespace video
	{

#ifdef ANDROID
		typedef OpenGLDriverAndroid GraphicsDriver;
#endif // ANDROID

	}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_GRAPHICS_DRIVER_GRAPHICSDRIVER_H_
