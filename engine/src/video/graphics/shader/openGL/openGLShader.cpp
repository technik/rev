//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/07
//----------------------------------------------------------------------------------------------------------------------
// Generic shader interface
#include "openGLShader.h"

#include <iostream>

#include <core/platform/fileSystem/file.h>
#include <video/graphics/driver/openGL/glew.h>

using namespace rev::core;

namespace rev {
	namespace video {

		//------------------------------------------------------------------------------------------------------------------
		bool detectShaderError(unsigned _shaderId, const char * _shaderName)
		{
			int status;
			glGetShaderiv(_shaderId, GL_COMPILE_STATUS, &status);
			if (status == GL_TRUE)
			{
				return false;
			}
			else
			{
				char buffer[1024];
				int len;
				glGetShaderInfoLog(_shaderId, 1024, &len, buffer);
				buffer[len] = '\0';
				std::cout << "Error compiling shader \"" << _shaderName << "\"\n"
					<< buffer << "\n";
				return true;
			}
		}

		//----------------------------------------------------------------------------------------------------------------------
		unsigned createShader(const char* _fileName, GLenum _type) {
			unsigned shaderId = glCreateShader(_type);
			File* shaderFile = new File(_fileName);
			const char * code[1];
			code[0] = shaderFile->bufferAsText();
			glShaderSource(shaderId, 1, code, 0); // Attach source
			glCompileShader(shaderId); // Compile
			if (detectShaderError(shaderId, _fileName))
			{
				glDeleteShader(shaderId);
				return 0;
			}
			return shaderId;
		}

		//----------------------------------------------------------------------------------------------------------------------
		OpenGLShader::OpenGLShader(const string& _shaderName) {
			string vtxShaderName = _shaderName + ".vtx";
			mVtx = createShader(vtxShaderName.c_str(), GL_VERTEX_SHADER);
			if (!mVtx) {
				std::cout << "Error creating vertex shader from " << vtxShaderName << "\n";
				return; // Failure
			}
			string pxlShaderName = _shaderName + ".pxl";
			mPxl = createShader(pxlShaderName.c_str(), GL_FRAGMENT_SHADER);
			if (!mPxl) {
				std::cout << "Error creating pixel shader from " << pxlShaderName << "\n";
				return; // Failure
			}
			mProgram = glCreateProgram();
			glAttachShader(mProgram, mVtx);
			glAttachShader(mProgram, mPxl);
			glBindAttribLocation(mProgram, 0, "vertex");
			glBindAttribLocation(mProgram, 1, "normal");
			glLinkProgram(mProgram);
		}

		//----------------------------------------------------------------------------------------------------------------------
		OpenGLShader::~OpenGLShader() {
			glDeleteProgram(mProgram);
			glDeleteShader(mVtx);
			glDeleteShader(mPxl);
		}
	}
}