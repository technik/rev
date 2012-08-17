//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 10th, 2012
//----------------------------------------------------------------------------------------------------------------------
// OpenGL 20 based 3d drivers' interface

#ifndef _REV_VIDEO_3DDRIVER_OPENGL20_3DDRIVEROPENGL20_H_
#define _REV_VIDEO_3DDRIVER_OPENGL20_3DDRIVEROPENGL20_H_

#ifdef WIN32
#include <Windows.h>
#endif // WIN32

#include <gl/GL.h>
#include <glext.h>

#include "../driver3d.h"

namespace rev { namespace video
{
	class Driver3dOpenGL20 : public Driver3d
	{
	public:
		Driver3dOpenGL20();
		virtual ~Driver3dOpenGL20() {} // Virtual destructor

		// Note: position correspond to the lower left corner of the rectangle and the window, starting at (0,0)
		void	setViewport			(const math::Vec2i& position, const math::Vec2u& size);
		void	clearZBuffer		();
		void	clearColorBuffer	();
		void	setClearColor		(const Color&);

		void	flush				();
		void	finishFrame			();

		//

	private: // Methods for internal use
		void	loadOpenGLExtensions();

	private: // Abstract methods
		virtual void	swapBuffers	() = 0;

	public:
		// OpenGL extensions
		void	glAttachShader	(GLuint _program, GLuint _shader) { mAttachShader(_program, _shader); }
		GLuint	glCreateProgram	()	{ return mCreateProgram(); }
		void	glLinkProgram	(GLuint _program)	{ mLinkProgram(_program); }

	private:
		PFNGLATTACHSHADERPROC	mAttachShader;
		PFNGLCREATEPROGRAMPROC	mCreateProgram;
		PFNGLLINKPROGRAMPROC	mLinkProgram;
	};

}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_3DDRIVER_OPENGL20_3DDRIVEROPENGL20_H_
