//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Interface with OpenGL
#pragma once

#ifdef ANDROID
	#include <EGL/egl.h>
	#include <GLES3/gl3.h>
#define __gl2_h_ // This is a workaround for lower api levels
	#include <GLES2/gl2ext.h>
	#include <GLES3/gl3platform.h>
#else // !ANDROID
#	include "glew.h"
#	ifdef _WIN32
#		include <Windows.h>
#	endif
#	include <GL/gl.h>
#	ifdef _WIN32
#		include "glew.h"
#	else
#		include <GL/glew.h>
#	endif // !_WIN32
#endif // !ANDROID
