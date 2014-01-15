//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 10th, 2012
//----------------------------------------------------------------------------------------------------------------------
// OpenGL 2.1 based 3d drivers' interface

#ifndef _REV_VIDEO_3DDRIVER_OPENGL21_3DDRIVEROPENGL21_H_
#define _REV_VIDEO_3DDRIVER_OPENGL21_3DDRIVEROPENGL21_H_

#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32

#include <cstdint>
#include <map>

#include <GL/gl.h>
#include "glext.h"

#include "../driver3d.h"

namespace rev { namespace video
{
	class VtxShaderOpenGL21;

	class Driver3dOpenGL21 : public Driver3d
	{
	public:
		Driver3dOpenGL21();
		virtual ~Driver3dOpenGL21() {} // Virtual destructor

		void init();
		unsigned	loadShader					(const char* _vtx, const char* _pxl);
		void	setShader					(unsigned _program);

		// Note: position correspond to the lower left corner of the rectangle and the window, starting at (0,0)
		void		setViewport				(const math::Vec2i& position, const math::Vec2u& size);
		void		clearZBuffer			();
		void		clearColorBuffer		();
		void		setClearColor			(const Color&);
		void		setZCompare				(bool _enable);

		void		flush					();
		void		finishFrame				();

		// --- Vertex config and surface config ---
		void		setVtxConfig			(VtxConfig* ) {}
		void		surfaceConfig			(SrfConfig* ) {}

		int			getUniformLocation		(const char* _uniformName);

		// --- Attributes and uniforms
		void		setAttribBuffer			(unsigned _id, unsigned _nElements, const float * _buffer);
		void		setAttribBuffer			(unsigned _id, unsigned _nElements, const uint16_t* _buffer);
		void		setAttribBuffer			(unsigned _id, unsigned _nElements, const int16_t* _buffer);
		void		setAttribBuffer			(unsigned _id, unsigned _nElements, const math::Vec2f* _buffer);
		void		setAttribBuffer			(unsigned _id, unsigned _nElements, const math::Vec3f* _buffer);

		void		setUniform				(int _id, float _value);
		void		setUniform				(int _id, const math::Vec2f& _value);
		void		setUniform				(int _id, const math::Vec3f& _value);
		void		setUniform				(int _id, const math::Mat44f& _value);
		void		setUniform				(int _id, const Color& _value);
		void		setUniform				(int _id, const Image* _value);

		// --- Draw ---
		void		drawIndexBuffer			(int _nIndices, unsigned short * _indices, Driver3d::EPrimitiveType _primitive);

	protected: // Methods for internal use
		void	loadOpenGLExtensions		();

	private:
		bool	detectShaderError			(unsigned _shaderId, const char * _shaderName);
		void	unregisterTexture			(const Image* _image);
		unsigned registerTexture			(const Image* _image);

		typedef std::pair<const uint8_t*, unsigned> textureReg;
		GLuint							mProgram;
		std::map<const Image*, textureReg>	mTextures;

	private: // Abstract methods
		virtual void	swapBuffers			() = 0;

	public:
		// OpenGL extensions
		void	glAttachShader				(GLuint _program, GLuint _shader)	{ mAttachShader(_program, _shader); }
		void	glBindAttribLocation		(GLuint _program, GLuint _index, const GLchar* _name);
		void	glCompileShader				(GLuint _shader)					{ mCompileShader(_shader);			}
		GLuint	glCreateProgram				()									{ return mCreateProgram();			}
		GLuint	glCreateShader				(GLenum _shaderType)				{ return mCreateShader(_shaderType);}
		void	glDeleteProgram				(GLuint _program)					{ mDeleteProgram(_program);			}
		void	glDeleteShader				(GLuint _shader)					{ mDeleteShader(_shader);			}
		void	glDisableVertexAttribArray	(GLuint _index);
		void	glEnableVertexAttribArray	(GLuint _index);
		GLint	glGetUniformLocation		(GLuint _program, const GLchar* _uniform);
		void	glGetShaderInfoLog			(GLuint _shader, GLsizei _maxLen, GLsizei* _length, GLchar* _infoLog);
		void	glGetShaderiv				(GLuint _shader, GLenum _paramName, GLint* _params);
		void	glLinkProgram				(GLuint _program)					{ mLinkProgram(_program);			}
		void	glShaderSource				(GLuint _shader, GLsizei _count, const GLchar ** _string, const GLint * _length);
		void	glUniform1i					(GLint _uniform, int _value);
		void	glUniform1f					(GLint _uniform, float _value);
		void	glUniform2f					(GLint _uniform, float _f0, float _f1);
		void	glUniform3f					(GLint _uniform, float _f0, float _f1, float _f2);
		void	glUniform4f					(GLint _uniform, float _f0, float _f1, float _f2, float _f3);
		void	glUniformMatrix4fv			(GLint _uniform, GLsizei _count, GLboolean _transpose, const GLfloat* _value);
		void	glUseProgram				(GLuint _program)					{ mUseProgram(_program);			}
		void	glVertexAttribPointer		(GLuint _idx, GLint _size, GLenum _type, bool _normalized, GLsizei _stride,
				const GLvoid * _pointer);


	private:
		// OpenGL extensions
		PFNGLATTACHSHADERPROC				mAttachShader;
		PFNGLBINDATTRIBLOCATIONPROC			mBindAttribLocation;
		PFNGLCOMPILESHADERPROC				mCompileShader;
		PFNGLCREATEPROGRAMPROC				mCreateProgram;
		PFNGLCREATESHADERPROC				mCreateShader;
		PFNGLDELETEPROGRAMPROC				mDeleteProgram;
		PFNGLDELETESHADERPROC				mDeleteShader;
		PFNGLDISABLEVERTEXATTRIBARRAYPROC	mDisableVertexAttribArray;
		PFNGLENABLEVERTEXATTRIBARRAYPROC	mEnableVertexAttribArray;
		PFNGLGETUNIFORMLOCATIONPROC			mGetUniformLocation;
		PFNGLGETSHADERINFOLOGPROC			mGetShaderInfoLog;
		PFNGLGETSHADERIVPROC				mGetShaderiv;
		PFNGLLINKPROGRAMPROC				mLinkProgram;
		PFNGLSHADERSOURCEPROC				mShaderSource;
		PFNGLUNIFORM1IPROC					mUniform1i;
		PFNGLUNIFORM1FPROC					mUniform1f;
		PFNGLUNIFORM2FPROC					mUniform2f;
		PFNGLUNIFORM3FPROC					mUniform3f;
		PFNGLUNIFORM4FPROC					mUniform4f;
		PFNGLUNIFORMMATRIX4FVPROC			mUniformMatrix4fv;
		PFNGLUSEPROGRAMPROC					mUseProgram;
		PFNGLVERTEXATTRIBPOINTERPROC		mVertexAttribPointer;
	};

}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_3DDRIVER_OPENGL21_3DDRIVEROPENGL21_H_
