//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/07
//----------------------------------------------------------------------------------------------------------------------
// Generic shader interface
#include "openGLShader.h"

#include <video/graphics/driver/openGL/glew.h>

using namespace rev::core;

namespace rev {
	namespace video {

		//----------------------------------------------------------------------------------------------------------------------
		unsigned createShader(const char* _fileName, GLenum _type) {

			unsigned shaderId = glCreateShader(_type);
		}

		//----------------------------------------------------------------------------------------------------------------------
		OpenGLShader::OpenGLShader(const string& _shaderName) {
			string vtxShaderName = _shaderName + ".vtx";
			string pxlShaderName = _shaderName + ".pxl";

			mProgram = glCreateProgram();
		}
	}
}