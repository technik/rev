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

#include <types/color/color.h>
#include <types/shader/openGL21/shaderOpenGL21.h>

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
	void Driver3dOpenGL21::loadOpenGLExtensions()
	{
		mAttachShader	= (PFNGLATTACHSHADERPROC)loadExtension("glAttachShader");
		mCompileShader	= (PFNGLCOMPILESHADERPROC)loadExtension("glCompileShader");
		mCreateProgram	= (PFNGLCREATEPROGRAMPROC)loadExtension("glCreateProgram");
		mCreateShader	= (PFNGLCREATESHADERPROC)loadExtension("glCreateShader");
		mDeleteProgram	= (PFNGLDELETEPROGRAMPROC)loadExtension("glDeleteProgram");
		mDeleteShader	= (PFNGLDELETESHADERPROC)loadExtension("glDeleteShader");
		mLinkProgram	= (PFNGLLINKPROGRAMPROC)loadExtension("glLinkProgram");
		mUseProgram		= (PFNGLUSEPROGRAMPROC)loadExtension("glUseProgram");
		mShaderSource	= (PFNGLSHADERSOURCEPROC)loadExtension("glShaderSource");
	};

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::glShaderSource(GLuint _shader, GLsizei _count, const GLchar ** _string, const GLint * _length)
	{
		mShaderSource(_shader, _count, _string, _length);
	}

}	// namespace video
}	// namespace rev