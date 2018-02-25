//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Interface with OpenGL
#pragma once

#ifdef ANDROID
	#include <android/native_activity.h>
	#include <EGL/egl.h>
	#include <GLES3/gl3.h>
#define __gl2_h_ // This is a workaround for lower api levels
	#include <GLES2/gl2ext.h>
	#include <GLES3/gl3platform.h>
#else // !ANDROID
	#define GLEW_STATIC
	#include <GL/glew.h>
	#ifdef _WIN32
		#include <GL/wglew.h>
	#endif // _WIN32
#include <glfw/glfw3.h>

#endif // !ANDROID
