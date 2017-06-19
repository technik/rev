//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Interface with graphics hardware

#ifndef _REV_VIDEO_GRAPHICS_DRIVER_GRAPHICSDRIVER_H_
#define _REV_VIDEO_GRAPHICS_DRIVER_GRAPHICSDRIVER_H_

#ifdef ANDROID
	#include "openGL/android/openGLDriverAndroid.h"
//#include "vulkan/vulkanDriver.h"
#else // !ANDROID
	#include "openGL/openGLDriver.h"
#endif // !ANDROID

namespace rev {
	namespace video
	{
#ifdef ANDROID
		//typedef VulkanDriver GraphicsDriver;
		typedef OpenGLDriverAndroid	GraphicsDriver;
#else // !ANDROID
		typedef OpenGLDriver GraphicsDriver; 
		//typedef VulkanDriver	GraphicsDriver;
#endif // !_WIN32

	}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_GRAPHICS_DRIVER_GRAPHICSDRIVER_H_
