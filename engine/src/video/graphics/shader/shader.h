//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/07
//----------------------------------------------------------------------------------------------------------------------
// Generic shader interface
#ifndef _REV_VIDEO_GRAPHICS_SHADER_SHADER_H_
#define _REV_VIDEO_GRAPHICS_SHADER_SHADER_H_

#ifdef OPENGL_45
#include "openGL/openGLShader.h"
#endif // OPENGL_45

namespace rev {
	namespace video {
#ifdef OPENGL_45
		typedef OpenGLShader Shader;
#endif // OPENGL_45
	}
}

#endif // _REV_VIDEO_GRAPHICS_SHADER_SHADER_H_