//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 10th, 2012
//----------------------------------------------------------------------------------------------------------------------
// OpenGL 2.1 based 3d drivers' interface

#ifndef _REV_VIDEO_3DDRIVER_OPENGL21_3DDRIVEROPENGL21_H_
#define _REV_VIDEO_3DDRIVER_OPENGL21_3DDRIVEROPENGL21_H_

#ifdef WIN32
#include <Windows.h>
#endif // WIN32

#include <gl/GL.h>
#include "glext.h"

#include "../driver3d.h"

namespace rev { namespace video
{
	class Driver3dOpenGL21 : public Driver3d
	{
	public:
		Driver3dOpenGL21();
		virtual ~Driver3dOpenGL21() {} // Virtual destructor

		// Note: position correspond to the lower left corner of the rectangle and the window, starting at (0,0)
		void	setViewport			(const math::Vec2i& position, const math::Vec2u& size);
		void	clearZBuffer		();
		void	clearColorBuffer	();
		void	setClearColor		(const Color&);

		void	flush				();
		void	finishFrame			();

		void	setShader			(const Shader * _shader);

	private: // Methods for internal use
		void	loadOpenGLExtensions();

	private: // Abstract methods
		virtual void	swapBuffers	() = 0;

	public:
		// OpenGL extensions
		void	glAttachShader	(GLuint _program, GLuint _shader)	{ mAttachShader(_program, _shader); }
		void	glCompileShader	(GLuint _shader)					{ mCompileShader(_shader);			}
		GLuint	glCreateProgram	()									{ return mCreateProgram();			}
		GLuint	glCreateShader	(GLenum _shaderType)				{ return mCreateShader(_shaderType);}
		void	glDeleteProgram	(GLuint _program)					{ mDeleteProgram(_program);			}
		void	glDeleteShader	(GLuint _shader)					{ mDeleteShader(_shader);			}
		void	glLinkProgram	(GLuint _program)					{ mLinkProgram(_program);			}
		void	glUseProgram	(GLuint _program)					{ mUseProgram(_program);			}
		void	glShaderSource	(GLuint _shader, GLsizei _count, const GLchar ** _string, const GLint * _length);


	private:
		// OpenGL extensions
		PFNGLATTACHSHADERPROC	mAttachShader;
		PFNGLCOMPILESHADERPROC	mCompileShader;
		PFNGLCREATEPROGRAMPROC	mCreateProgram;
		PFNGLCREATESHADERPROC	mCreateShader;
		PFNGLDELETEPROGRAMPROC	mDeleteProgram;
		PFNGLDELETESHADERPROC	mDeleteShader;
		PFNGLLINKPROGRAMPROC	mLinkProgram;
		PFNGLUSEPROGRAMPROC		mUseProgram;
		PFNGLSHADERSOURCEPROC	mShaderSource;
	};

}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_3DDRIVER_OPENGL21_3DDRIVEROPENGL21_H_
