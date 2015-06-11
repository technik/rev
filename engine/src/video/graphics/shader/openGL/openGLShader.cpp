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
				char buffer[4096];
				int len;
				glGetShaderInfoLog(_shaderId, 1024, &len, buffer);
				if(len > 4096) len = 4096;
				buffer[len] = '\0';
				std::cout << "Error compiling shader \"" << _shaderName << "\"\n"
					<< buffer << "\n";
				return true;
			}
		}

		//----------------------------------------------------------------------------------------------------------------------
		unsigned createShader(const char* _fileName, GLenum _type) {
			unsigned shaderId = glCreateShader(_type);
			File* shaderFile = File::openExisting(_fileName);
			if(!shaderFile)
			{
				std::cout << "Error: Unable to open shader file \"" << _fileName << "\"\n";
				return 0;
			}
			shaderFile->readAll();
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
		OpenGLShader::OpenGLShader(unsigned _vtx, unsigned _pxl)
			:mVtx(_vtx)
			,mPxl(_pxl)
		{
			assert(_vtx);
			assert(_pxl);
			mProgram = glCreateProgram();
			glAttachShader(mProgram, mVtx);
			glAttachShader(mProgram, mPxl);
			glBindAttribLocation(mProgram, 0, "vertex");
			glBindAttribLocation(mProgram, 1, "normal");
			glBindAttribLocation(mProgram, 2, "uv");
			glLinkProgram(mProgram);
		}

		//----------------------------------------------------------------------------------------------------------------------
		OpenGLShader::~OpenGLShader() {
			glDeleteProgram(mProgram);
			glDeleteShader(mVtx);
			glDeleteShader(mPxl);
		}

		//----------------------------------------------------------------------------------------------------------------------
		OpenGLShader* OpenGLShader::loadFromFiles(const core::string& _vtxName, const core::string& _pxlName) {
			unsigned vtx = createShader(_vtxName.c_str(), GL_VERTEX_SHADER);
			if (!vtx) {
				std::cout << "Error creating vertex shader from " << _vtxName << "\n";
				return nullptr; // Failure
			}
			unsigned pxl = createShader(_pxlName.c_str(), GL_FRAGMENT_SHADER);
			if (!pxl) {
				std::cout << "Error creating pixel shader from " << _pxlName << "\n";
				return nullptr; // Failure
			}
			return new OpenGLShader(vtx,pxl);
		}

		//----------------------------------------------------------------------------------------------------------------------
		void OpenGLShader::loadFromFiles(const core::string& _vtxName, const core::string& _pxlName, OpenGLShader& _dst) {
			unsigned vtx = createShader(_vtxName.c_str(), GL_VERTEX_SHADER);
			if (!vtx) {
				std::cout << "Error creating vertex shader from " << _vtxName << "\n";
				new(&_dst)OpenGLShader(); // Failure
			}
			unsigned pxl = createShader(_pxlName.c_str(), GL_FRAGMENT_SHADER);
			if (!pxl) {
				std::cout << "Error creating pixel shader from " << _pxlName << "\n";
				new(&_dst)OpenGLShader(); // Failure
			}
			new(&_dst) OpenGLShader(vtx,pxl);
		}
	}
}