////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on September 11th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OpenGL based video driver

#ifndef _REV_REVVIDEO_VIDEODRIVER_OPENGL_VIDEODRIVEROPENGL_H_
#define _REV_REVVIDEO_VIDEODRIVER_OPENGL_VIDEODRIVEROPENGL_H_

// GL headers
#ifdef _linux
#define GL_GLEXT_PROTOTYPES
#endif // _linux
#include <GL/gl.h>
#ifdef _linux
#include <GL/glx.h>		// GL-X dependency
#include <X11/Xlib.h>	// X11 library
#endif // _linux
#include "glext.h"

// Engine headers
#include "revVideo/videoDriver/videoDriver.h"

namespace rev { namespace video
{
	class IVideoDriverOpenGL: public IVideoDriver
	{
	public:
		// -- Constructor & virtual destructor
		IVideoDriverOpenGL();
		virtual ~IVideoDriverOpenGL()	{}

		// ---- Render tasks ---- //
		void	setShader	(const int _shader);
		void	setRealAttribBuffer	(const int _attribId, const unsigned _nComponents, const void * const _buffer);
		void	drawIndexBuffer	(const int _nIndices, const unsigned short * _indices, const bool _strip);

	private:
		// Shaders internal management
		virtual int 	loadShader				(const char * _vtxName, const char * _pxlName);
		virtual	void	deleteShader			(const int _id);
		void			bindAttributes			(int _shader);

		// -- Extensions
		void			loadOpenGLExtensions	();
		// ---- OpenGL extension wrappers
		void			glUseProgram			(unsigned _programId);
		unsigned		glCreateProgram			();
		void			glDeleteProgram			(unsigned _program);
		unsigned		glCreateShader			(unsigned _shaderType);
		void			glDeleteShader			(unsigned _shader);
		void			glShaderSource			(unsigned _shader, int _count, const char ** _string, const int * _length);
		void			glCompileShader			(unsigned _shader);
		void			glAttachShader			(unsigned _program, unsigned _shader);
		void			glLinkProgram			(unsigned _program);
		void			glBindAttribLocation	(unsigned _program, unsigned _index, const char * _name);
		void			glVertexAttribPointer	(unsigned _idx, int _size, unsigned _type, bool _normalized,
												int _stride, const void * _pointer);

	private:
		// --- Internal state --- //
		int		mCurShader;

	private:
		// ---- pointers to openGL extensions ----
		// Shader creation and use
		PFNGLUSEPROGRAMPROC m_useProgram	;
		PFNGLCREATEPROGRAMPROC m_createProgram;
		PFNGLDELETEPROGRAMPROC m_deleteProgram;
		PFNGLCREATESHADERPROC m_createShader;
		PFNGLDELETESHADERPROC m_deleteShader;
		PFNGLSHADERSOURCEPROC m_shaderSource;
		PFNGLCOMPILESHADERPROC m_compileShader;
		PFNGLATTACHSHADERPROC m_attachShader;
		PFNGLLINKPROGRAMPROC m_linkProgram;
		// Data binding
		PFNGLBINDATTRIBLOCATIONPROC m_bindAttribLocation;
		PFNGLVERTEXATTRIBPOINTERPROC m_vertexAttribPointer;
	};
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_VIDEODRIVER_OPENGL_VIDEODRIVEROPENGL_H_
