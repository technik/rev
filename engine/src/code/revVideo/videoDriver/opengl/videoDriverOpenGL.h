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
#ifdef WIN32
#include <Windows.h>
#endif // WIN32
#include <GL/gl.h>
#ifdef _linux
#include <GL/glx.h>		// GL-X dependency
#include <X11/Xlib.h>	// X11 library
#endif // _linux
#include "glext.h"

// Engine headers
#include <vector.h>
#include <revVideo/videoDriver/videoDriver.h>

namespace rev { namespace video
{
	// Forward declarations
	class CColor;

	class IVideoDriverOpenGL: public IVideoDriver
	{
	public:
		// -- Constructor & virtual destructor
		IVideoDriverOpenGL();
		virtual ~IVideoDriverOpenGL()	{}

		// ---- Render tasks ---- //
		void	setModelViewMatrix	(const CMat34& _mv);
		void	setProjMatrix		(const CMat4& _proj);
		void	setShader			(const int _shader);
		int		getUniformId		(const char * _name) const;
		void	setRealAttribBuffer	(const int _attribId, unsigned _nElements, const unsigned _nComponents, const void * const _buffer);
		void	setUniform			(int _id, float _value);
		void	setUniform			(int _id, const CMat4& _value);
		void	setUniform			(int _id, const CColor& _value);
		void	setUniform			(int _id, const CVec3& _value);
		void	setUniform			(int _id, int _slot, const CTexture * _value);
		void	drawIndexBuffer		(const int _nIndices, const unsigned short * _indices,
									IVideoDriver::EPrimitiveType _primitive);
		
		void	setBackgroundColor	(const CColor& _color);

		virtual void	beginFrame	();
		void			initOpenGL	();
		virtual	void	endFrame	();
	private:
		// -- OpenGL specifics --
		virtual void	createOpenGLWindow		(const unsigned int _width, const unsigned int _heihgt) = 0;
		void			flushGeometryCache		();
		
		// --- Shader management -- //
		int				linkShader			(CVtxShader* _vtx, CPxlShader* _pxl);
		void			destroyShader		(int _id);
		int				loadVtxShader		(const char * _name);
		int				loadPxlShader		(const char * _name);
		void			releaseShader		(int _id);
		void			bindAttributes		(int _shader);

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
		void			glGetShaderiv			(unsigned _shader, unsigned _paramType, int * param) const;
		void			glGetShaderInfoLog		(unsigned _shader, unsigned _maxSize, int * length, char * buffer) const;
		void			glAttachShader			(unsigned _program, unsigned _shader);
		void			glLinkProgram			(unsigned _program);
		void			glBindAttribLocation	(unsigned _program, unsigned _index, const char * _name);
		int				glGetUniformLocation	(unsigned _program, const char* _name) const;
		void			glVertexAttribPointer	(unsigned _idx, int _size, unsigned _type, bool _normalized,
												int _stride, const void * _pointer);
		void			glEnableVertexAttribArray(unsigned _idx);
		void			glUniformMatrix4fv		(unsigned _location, int _count, bool _transpose, const float *_value);
		void			glUniform1f				(unsigned _location, float _f);
		void			glUniform3f				(unsigned _location, float _f0, float _f1, float _f3);
		void			glUniform4f				(unsigned _location, float _f0, float _f1, float _f2, float _f3);
		void			glUniform1i				(unsigned _location, int _i);

		void			glActiveTexture			(GLenum aTexture);

		// --- Debugging aids ---
		bool			detectShaderError			(unsigned shader) const;

	private:
		// --- Internal state --- //
		int				mCurShader;
		unsigned int	mScreenWidth;
		unsigned int	mScreenHeight;
		// Geometry cache
		int					mMVPUniformId;
		CMat34				mModelView;
		const CVec3*		mVertexBuffer;
		const CVec3*		mNormalBuffer;
		const CVec2*		mTexCoordBuffer;
		rtl::vector<CVec3>	mVertexCache;
		rtl::vector<CVec3>	mNormalCache;
		rtl::vector<CVec2>	mTexCoordCache;
		rtl::vector<u16>	mTriangleCache;
		rtl::vector<u16>	mTriStripCache;
		rtl::vector<u16>	mLineCache;
		rtl::vector<u16>	mLineStripCache;
		unsigned			m0Idx;	// Index where starts the geometry of the current renderable
		unsigned			mNVertices;

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
		PFNGLGETSHADERIVPROC m_getShaderiv;
		PFNGLGETSHADERINFOLOGPROC m_getShaderInfoLog;
		PFNGLATTACHSHADERPROC m_attachShader;
		PFNGLLINKPROGRAMPROC m_linkProgram;
		// Data binding
		PFNGLBINDATTRIBLOCATIONPROC m_bindAttribLocation;
		PFNGLGETUNIFORMLOCATIONPROC m_getUniformLocation;
		PFNGLVERTEXATTRIBPOINTERPROC m_vertexAttribPointer;
		PFNGLENABLEVERTEXATTRIBARRAYPROC m_enableVertexAttribArray;
		PFNGLUNIFORMMATRIX4FVPROC m_uniformMatrix4fv;
		PFNGLUNIFORM1FPROC m_uniform1f;
		PFNGLUNIFORM3FPROC m_uniform3f;
		PFNGLUNIFORM4FPROC m_uniform4f;
		PFNGLUNIFORM1IPROC m_uniform1i;
		PFNGLACTIVETEXTUREPROC m_activeTexture;
	};
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_VIDEODRIVER_OPENGL_VIDEODRIVEROPENGL_H_
