//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/07
//----------------------------------------------------------------------------------------------------------------------
// Generic shader interface

#include "openGLShader.h"

#include <iostream>

#include <core/platform/fileSystem/file.h>

using namespace rev::core;

namespace rev {
	namespace core {
		template<> video::OpenGLShader::Mgr* video::OpenGLShader::Mgr::sInstance = nullptr;
} }

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

		//------------------------------------------------------------------------------------------------------------------
		bool detectProgramError(unsigned _shaderId)
		{
			int status;
			glGetProgramiv(_shaderId, GL_LINK_STATUS, &status);
			if (status == GL_TRUE)
			{
				return false;
			}
			else
			{
				char buffer[4096];
				int len;
				glGetProgramInfoLog(_shaderId, 1024, &len, buffer);
				if (len > 4096) len = 4096;
				buffer[len] = '\0';
				std::cout << "Error linking shader\n"
					<< buffer << "\n";
				return true;
			}
		}

		//----------------------------------------------------------------------------------------------------------------------
		void logGlErrors() {
			GLenum error = glGetError();
			if (error) {
				switch (error) {
				case GL_INVALID_ENUM:
					std::cout << "GL_INVALID_ENUM\n";
					break;
				case GL_INVALID_VALUE:
					std::cout << "GL_INVALID_VALUE\n";
					break;
				case GL_INVALID_OPERATION:
					std::cout << "GL_INVALID_OPERATION\n";
					break;
				default:
					std::cout << "Other GL error\n";
					break;
				}
			}
		}

		//----------------------------------------------------------------------------------------------------------------------
		unsigned createShader(const char* _code, GLenum _type) {
			logGlErrors();
			unsigned shaderId = glCreateShader(_type);
			logGlErrors();
			char vtxDefine[] = "#define VERTEX_SHADER";
			char frgDefine[] = "#define FRAGMENT_SHADER";
			const char * code[2];
			code[0] = (_type == GL_VERTEX_SHADER)?vtxDefine:frgDefine;
			code[1] = _code;
			logGlErrors();
			glShaderSource(shaderId, 2, code, 0); // Attach source
			logGlErrors();
			glCompileShader(shaderId); // Compile
			logGlErrors();
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
			if (detectProgramError(mProgram)) {
				glDeleteProgram(mProgram);
				mProgram = unsigned(-1);
			}
		}

		//----------------------------------------------------------------------------------------------------------------------
		OpenGLShader::~OpenGLShader() {
			//glDeleteProgram(mProgram);
			//glDeleteShader(mVtx);
			//glDeleteShader(mPxl);
		}

		//----------------------------------------------------------------------------------------------------------------------
		OpenGLShader* OpenGLShader::loadFromFile(const std::string& _fileName) {
			File sourceCode(_fileName);
			unsigned vtx = createShader(sourceCode.bufferAsText(), GL_VERTEX_SHADER);
			if (!vtx) {
				std::cout << "Error creating vertex shader from " << _fileName << "\n";
				return nullptr; // Failure
			}
			unsigned pxl = createShader(sourceCode.bufferAsText(), GL_FRAGMENT_SHADER);
			if (!pxl) {
				std::cout << "Error creating pixel shader from " << _fileName << "\n";
				return nullptr; // Failure
			}
			return new OpenGLShader(vtx,pxl);
		}

		//----------------------------------------------------------------------------------------------------------------------
		void OpenGLShader::loadFromFile(const std::string& _fileName, OpenGLShader& _dst) {
			File sourceCode(_fileName);
			if(!sourceCode.sizeInBytes())
				return;
			unsigned vtx = createShader(sourceCode.bufferAsText(), GL_VERTEX_SHADER);
			if (!vtx) {
				std::cout << "Error creating vertex shader from " << _fileName << "\n";
				return;
			}
			unsigned pxl = createShader(sourceCode.bufferAsText(), GL_FRAGMENT_SHADER);
			if (!pxl) {
				std::cout << "Error creating pixel shader from " << _fileName << "\n";
				return;
			}
			OpenGLShader newShader(vtx, pxl);
			if(newShader.program() != unsigned(-1))
				_dst = newShader;
		}
	}
}