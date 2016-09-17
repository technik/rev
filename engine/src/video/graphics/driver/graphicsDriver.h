//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/05
//----------------------------------------------------------------------------------------------------------------------
// Interface with graphics hardware

#ifndef _REV_VIDEO_GRAPHICS_DRIVER_GRAPHICSDRIVER_H_
#define _REV_VIDEO_GRAPHICS_DRIVER_GRAPHICSDRIVER_H_

#ifdef ANDROID
#include "openGL/android/openGLDriverAndroid.h"
#endif // ANDROID
#ifdef _WIN32
#include "openGL/windows/openGLDriverWindows.h"
#endif // _WIN32

namespace rev {
	namespace video
	{

#ifdef ANDROID
		typedef OpenGLDriverAndroid GraphicsDriver;
#endif // ANDROID
#ifdef _WIN32
		typedef OpenGLDriverWindows GraphicsDriver; 
#endif // _WIN32

	}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_GRAPHICS_DRIVER_GRAPHICSDRIVER_H_
