//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Interface with OpenGL
#ifndef _REV_VIDEO_GRAPHICS_DRIVER_OPENGL_OPENGL_H_
#define _REV_VIDEO_GRAPHICS_DRIVER_OPENGL_OPENGL_H_

#ifdef ANDROID
	#include <EGL/egl.h>
	#include <GLES3/gl3.h>
	#include <GLES3/gl3ext.h>
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


#endif // _REV_VIDEO_GRAPHICS_DRIVER_OPENGL_OPENGL_H_