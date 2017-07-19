//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Interface with graphics hardware

#pragma once

#ifdef REV_USE_VULKAN
#include "vulkan/vulkanDriver.h"
#else // !REV_USE_VULKAN
	#ifdef ANDROID
		#include "openGL/android/openGLDriverAndroid.h"
	#else // !ANDROID
		#ifdef _WIN32
			#include "openGL/windows/openGLDriverWindows.h"
		#else // !_WIN32
			#include "openGL/openGLDriver.h"
		#endif // !_WIN32
	#endif // !ANDROID
#endif // !REV_USE_VULKAN

namespace rev {
	namespace video
	{
#ifdef ANDROID
		//typedef VulkanDriver GraphicsDriver;
		typedef OpenGLDriverAndroid	GraphicsDriver;
#endif // ANDROID
#ifdef _WIN32
	#if REV_USE_VULKAN
		typedef VulkanDriver	GraphicsDriver;
	#endif // REV_USE_VULKAN
	#ifdef OPENGL_45
		typedef OpenGLDriverWindows GraphicsDriver; 
	#endif // OPENGL_45
#endif // _WIN32

	}	// namespace video
}	// namespace rev

