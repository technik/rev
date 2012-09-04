//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 10th, 2012
//----------------------------------------------------------------------------------------------------------------------
// OpenGL 2.1 based 3d drivers' interface

#include "driver3dOpenGL21.h"

#ifdef WIN32
#include <Windows.h>
#endif // WIN32

#include <GL/gl.h>
#include "glext.h"

#include <file/file.h>
#include <types/color/color.h>
#include <types/shader/pixel/openGL21/pxlShaderOpenGL21.h>
#include <types/shader/openGL21/shaderOpenGL21.h>
#include <types/shader/vertex/openGL21/vtxShaderOpenGL21.h>

using namespace rev::math;

#ifdef WIN32
#define loadExtension( a ) wglGetProcAddress( a )
#endif // WIN32

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	Driver3dOpenGL21::Driver3dOpenGL21()
	{
		loadOpenGLExtensions();
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::setViewport(const Vec2i& _position, const Vec2u& _size)
	{
		glViewport(GLint(_position.x),
					GLint(_position.y),
					GLsizei(_size.x),
					GLsizei(_size.y));
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::clearZBuffer()
	{
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::clearColorBuffer()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::setClearColor(const Color& _clr)
	{
		glClearColor(GLclampf(_clr.r), GLclampf(_clr.g), GLclampf(_clr.b), GLclampf(_clr.a));
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::flush()
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::finishFrame()
	{
		flush();
		swapBuffers();
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::setShader(const Shader * _shd)
	{
		const ShaderOpenGL21 * shader = static_cast<const ShaderOpenGL21*>(_shd);
		glUseProgram(shader->id());
	}

	//------------------------------------------------------------------------------------------------------------------
	PxlShader * Driver3dOpenGL21::createPxlShader(const char * _fileName)
	{
		File * shaderFile = File::open(_fileName, false);
		unsigned shaderId = glCreateShader(GL_VERTEX_SHADER);
		const char * code[1];
		code[0] = shaderFile->bufferAsText();
		glShaderSource(shaderId, 1, code, 0); // Attach source
		glCompileShader(shaderId); // Compile
		if(!detectShaderError(shaderId, _fileName))
			return new PxlShaderOpenGL21(shaderId);
		else
		{
			glDeleteShader(shaderId);
			return nullptr;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	Shader * Driver3dOpenGL21::createShader(VtxShader* _vtx, PxlShader * _pxl)
	{
		// Cast to the openGL version of the shaders
		if((nullptr == _vtx) || (nullptr == _pxl))
			return nullptr;
		const VtxShaderOpenGL21 * vtx21 = static_cast<const VtxShaderOpenGL21*>(_vtx);
		const PxlShaderOpenGL21 * pxl21 = static_cast<const PxlShaderOpenGL21*>(_pxl);

		// Create an OpenGL program and link the shaders
		GLuint program = glCreateProgram();
		glAttachShader(program, vtx21->id());
		glAttachShader(program, pxl21->id());
		glLinkProgram(program);

		return new ShaderOpenGL21(program);
	}

	//------------------------------------------------------------------------------------------------------------------
	VtxShader * Driver3dOpenGL21::createVtxShader(const char * _fileName)
	{File * shaderFile = File::open(_fileName, false);
		unsigned shaderId = glCreateShader(GL_FRAGMENT_SHADER);
		const char * code[1];
		code[0] = shaderFile->bufferAsText();
		glShaderSource(shaderId, 1, code, 0); // Attach source
		glCompileShader(shaderId); // Compile
		if(!detectShaderError(shaderId, _fileName))
			return new VtxShaderOpenGL21(shaderId);
		else
		{
			glDeleteShader(shaderId);
			return nullptr;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::loadOpenGLExtensions()
	{
		mAttachShader		= (PFNGLATTACHSHADERPROC)loadExtension("glAttachShader");
		mCompileShader		= (PFNGLCOMPILESHADERPROC)loadExtension("glCompileShader");
		mCreateProgram		= (PFNGLCREATEPROGRAMPROC)loadExtension("glCreateProgram");
		mCreateShader		= (PFNGLCREATESHADERPROC)loadExtension("glCreateShader");
		mDeleteProgram		= (PFNGLDELETEPROGRAMPROC)loadExtension("glDeleteProgram");
		mDeleteShader		= (PFNGLDELETESHADERPROC)loadExtension("glDeleteShader");
		mGetShaderInfoLog	= (PFNGLGETSHADERINFOLOGPROC)loadExtension("glGetShaderInfoLog");
		mGetShaderiv		= (PFNGLGETSHADERIVPROC)loadExtension("glGetShaderiv");
		mLinkProgram		= (PFNGLLINKPROGRAMPROC)loadExtension("glLinkProgram");
		mUseProgram			= (PFNGLUSEPROGRAMPROC)loadExtension("glUseProgram");
		mShaderSource		= (PFNGLSHADERSOURCEPROC)loadExtension("glShaderSource");
	};

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::glShaderSource(GLuint _shader, GLsizei _count, const GLchar ** _string, const GLint * _length)
	{
		mShaderSource(_shader, _count, _string, _length);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::glGetShaderInfoLog(GLuint _shader, GLsizei _maxLen, GLsizei* _length, GLchar* _infoLog)
	{
		mGetShaderInfoLog(_shader,_maxLen,_length,_infoLog);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::glGetShaderiv(GLuint _shader, GLenum _paramName, GLint* _params)
	{
		mGetShaderiv(_shader,_paramName,_params);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Driver3dOpenGL21::detectShaderError(unsigned _shaderId, const char * _shaderName)
	{
		int status;
		glGetShaderiv(_shaderId, GL_COMPILE_STATUS, &status);
		if(status == GL_TRUE)
		{
			return false;
		}
		else
		{
			char buffer[1024];
			int len;
			glGetShaderInfoLog(_shaderId, 1024, &len, buffer);
			buffer[len] = '\0';
			revLog() << "Error compiling shader \"" << _shaderName << "\"\n"
				<< buffer << "\n";
			revLog().flush();
			return true;
		}
	}

}	// namespace video
}	// namespace rev