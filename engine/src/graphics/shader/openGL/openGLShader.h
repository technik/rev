//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
// 2014/April/07
//----------------------------------------------------------------------------------------------------------------------
// Generic shader interface
#ifndef _REV_VIDEO_GRAPHICS_SHADER_OPENGL_OPENGLSHADER_H_
#define _REV_VIDEO_GRAPHICS_SHADER_OPENGL_OPENGLSHADER_H_

#include <string>
#include <core/resources/creator/virtualConstructor.h>
#include <core/resources/ownership/refLink.h>
#include <core/resources/resourceManager.h>

#include <video/graphics/driver/openGL/openGL.h>

namespace rev {
	namespace video {

		class OpenGLShader : public core::ManagedResource<std::string, OpenGLShader,
										core::VirtualConstructor<OpenGLShader, std::string>>
		{
		public:
			OpenGLShader() = default;
			OpenGLShader(unsigned _vtx, unsigned _pxl);
			~OpenGLShader();
			static OpenGLShader* loadFromFile(const std::string& _sourceFileName);
			static void loadFromFile(const std::string& _sourceFileName, OpenGLShader& _dst);
			inline unsigned program() const { return mProgram; }

		private:
			unsigned mProgram = 0;
			unsigned mVtx = 0;
			unsigned mPxl = 0;
		};
	}
}

#endif // _REV_VIDEO_GRAPHICS_SHADER_OPENGL_OPENGLSHADER_H_