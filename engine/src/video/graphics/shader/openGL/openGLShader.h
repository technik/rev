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
			OpenGLShader() = default;
			OpenGLShader(unsigned _vtx, unsigned _pxl);
			~OpenGLShader();
			static OpenGLShader* loadFromFiles(const core::string& _vtxName, const core::string& _pxlName);
			static void loadFromFiles(const core::string& _vtxName, const core::string& _pxlName, OpenGLShader& _dst);
			inline unsigned program() const { return mProgram; }
		private:
			unsigned mProgram = 0;
			unsigned mVtx = 0;
			unsigned mPxl = 0;
		};
	}
}

#endif // _REV_VIDEO_GRAPHICS_SHADER_OPENGL_OPENGLSHADER_H_