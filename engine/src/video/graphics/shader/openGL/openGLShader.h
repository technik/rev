//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/07
//----------------------------------------------------------------------------------------------------------------------
// Generic shader interface
#ifndef _REV_VIDEO_GRAPHICS_SHADER_OPENGL_OPENGLSHADER_H_
#define _REV_VIDEO_GRAPHICS_SHADER_OPENGL_OPENGLSHADER_H_

#include "../shader.h"
#include <core/types.h>

namespace rev {
	namespace video {
		class OpenGLShader : public Shader {
		public:
			OpenGLShader(const core::string&);
			inline unsigned program() const { return mProgram; }
		private:
			unsigned mProgram;
			unsigned mVtx;
			unsigned mIdx;
		};
	}
}

#endif // _REV_VIDEO_GRAPHICS_SHADER_OPENGL_OPENGLSHADER_H_