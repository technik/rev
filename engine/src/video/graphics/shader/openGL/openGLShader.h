//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/07
//----------------------------------------------------------------------------------------------------------------------
// Generic shader interface
#ifndef _REV_VIDEO_GRAPHICS_SHADER_OPENGL_OPENGLSHADER_H_
#define _REV_VIDEO_GRAPHICS_SHADER_OPENGL_OPENGLSHADER_H_

#include <string>

namespace rev {
	namespace video {

		class OpenGLShader {
		public:
			OpenGLShader() = default;
			OpenGLShader(unsigned _vtx, unsigned _pxl);
			~OpenGLShader();
			static OpenGLShader* loadFromFiles(const std::string& _vtxName, const std::string& _pxlName);
			static void loadFromFiles(const std::string& _vtxName, const std::string& _pxlName, OpenGLShader& _dst);
			inline unsigned program() const { return mProgram; }

		private:
			unsigned mProgram = 0;
			unsigned mVtx = 0;
			unsigned mPxl = 0;
		};
	}
}

#endif // _REV_VIDEO_GRAPHICS_SHADER_OPENGL_OPENGLSHADER_H_