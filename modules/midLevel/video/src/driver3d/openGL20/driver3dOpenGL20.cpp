//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 10th, 2012
//----------------------------------------------------------------------------------------------------------------------
// OpenGL 20 based 3d drivers' interface

#include "driver3dOpenGL20.h"

#ifdef WIN32
#include <Windows.h>
#endif // WIN32

#include <GL/gl.h>
#include "glext.h"

#include <types/color/color.h>

using namespace rev::math;

#ifdef WIN32
#define loadExtension( a ) wglGetProcAddress( a )
#endif // WIN32

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	Driver3dOpenGL20::Driver3dOpenGL20()
	{
		loadOpenGLExtensions();
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL20::setViewport(const Vec2i& _position, const Vec2u& _size)
	{
		glViewport(GLint(_position.x),
					GLint(_position.y),
					GLsizei(_size.x),
					GLsizei(_size.y));
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL20::clearZBuffer()
	{
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL20::clearColorBuffer()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL20::setClearColor(const Color& _clr)
	{
		glClearColor(GLclampf(_clr.r), GLclampf(_clr.g), GLclampf(_clr.b), GLclampf(_clr.a));
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL20::flush()
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL20::finishFrame()
	{
		flush();
		swapBuffers();
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL20::loadOpenGLExtensions()
	{
		mAttachShader	= (PFNGLATTACHSHADERPROC)loadExtension("glAttachShader");
		mCreateProgram	= (PFNGLCREATEPROGRAMPROC)loadExtension("glCreateProgram");
		mLinkProgram	= (PFNGLLINKPROGRAMPROC)loadExtension("glLinkProgram");
	};

}	// namespace video
}	// namespace rev