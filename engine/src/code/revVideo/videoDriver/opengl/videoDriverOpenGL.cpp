////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on September 11th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OpenGL based video driver

// Engine headers
#include "videoDriverOpenGL.h"

#include "revCore/codeTools/assert/assert.h"
#include "revCore/file/file.h"
#include "revCore/math/matrix.h"
#include "revVideo/color/color.h"
#include "revVideo/videoDriver/shader/pxlShader.h"
#include "revVideo/videoDriver/shader/vtxShader.h"

#ifdef _linux
#define loadExtension( a ) glXGetProcAddressARB((const GLubyte*) (a))
#endif // _linux
#ifdef WIN32
#define loadExtension( a ) wglGetProcAddress( a )
#endif // WIN32

using namespace rev::codeTools;

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	IVideoDriverOpenGL::IVideoDriverOpenGL():
		// Internal state and caches
		mCurShader(-1),
		mScreenWidth(800),
		mScreenHeight(480)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::setShader(const int _shader)
	{
		revAssert(_shader >= 0);
		if(_shader != mCurShader) // Check cache
		{
			mCurShader = _shader;
			glUseProgram(unsigned(mCurShader));
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	int IVideoDriverOpenGL::getUniformId(const char * _name) const
	{
		return glGetUniformLocation(mCurShader, _name);
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::setRealAttribBuffer(const int _attribId, const unsigned _nComponents, const void * const _buffer)
	{
		// Assert incomming data si valid
		revAssert(_nComponents && (_nComponents < 5)); // [0,4] reals per buffer element
		revAssert(_attribId >= 0); // Valid id
		revAssert(0 != _buffer); // Non-null buffer
		// Pass array to OpenGL
#if defined (_linux) || defined (WIN32)
		glVertexAttribPointer(_attribId, _nComponents, GL_FLOAT, false, 0, _buffer);
		glEnableVertexAttribArray(_attribId);
#else
		revAssert(false); // Does current platform use GL_FLOAT for TReal?
#endif
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::setUniform(int _id, const CMat4& _value)
	{
		m_uniformMatrix4fv(_id, 1, true, reinterpret_cast<const float*>(_value.m));
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::setUniform(int _id, const CColor& _value)
	{
		glUniform4f(_id, _value.r(), _value.g(), _value.b(), _value.a());
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::drawIndexBuffer(const int _nIndices, const unsigned short int * _indices, const bool _strip)
	{
		glDrawElements(_strip?GL_TRIANGLE_STRIP:GL_TRIANGLES, _nIndices, GL_UNSIGNED_SHORT, _indices);
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::setBackgroundColor(const CColor& _color)
	{
		glClearColor(_color.r(), _color.g(), _color.b(), _color.a());
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::beginFrame()
	{
		// Clear current buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, mScreenWidth, mScreenHeight);
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::initOpenGL()
	{
		// Load required openGL extensions
		loadOpenGLExtensions();
		// Configure the OpenGL context for rendering
		glClearColor(0.0, 0.0, 0.0, 0.0);
	}

	//------------------------------------------------------------------------------------------------------------------
	int IVideoDriverOpenGL::linkShader(CVtxShader * _vtx, CPxlShader * _pxl)
	{
		int program = glCreateProgram();
		glAttachShader(program, _vtx->id());
		glAttachShader(program, _pxl->id());
		bindAttributes(program);
		glLinkProgram(program);
		return program;
	}

	//------------------------------------------------------------------------------------------------------------------
	int IVideoDriverOpenGL::loadVtxShader(const char * _name)
	{
		unsigned shader = glCreateShader(GL_VERTEX_SHADER);
		const char * fileBuffer = bufferFromFile(_name);
		glShaderSource(shader, 1, &fileBuffer, 0); // Attach source
		glCompileShader(shader); // Compile
		delete[] fileBuffer;
		return int(shader);
	}

	//------------------------------------------------------------------------------------------------------------------
	int IVideoDriverOpenGL::loadPxlShader(const char * _name)
	{
		unsigned shader = glCreateShader(GL_FRAGMENT_SHADER);
		const char * fileBuffer = bufferFromFile(_name);
		glShaderSource(shader, 1, &fileBuffer, 0); // Attach source
		glCompileShader(shader); // Compile
		delete[] fileBuffer;
		return int(shader);
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::bindAttributes(int _shader)
	{
		glBindAttribLocation(_shader, eVertex, "vertex");
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::loadOpenGLExtensions()
	{
		m_useProgram = (PFNGLUSEPROGRAMPROC)loadExtension("glUseProgram");
		m_createProgram = (PFNGLCREATEPROGRAMPROC)loadExtension("glCreateProgram");
		m_deleteProgram = (PFNGLDELETEPROGRAMPROC)loadExtension("glDelteProgram");
		m_createShader = (PFNGLCREATESHADERPROC)loadExtension("glCreateShader");
		m_deleteShader = (PFNGLDELETESHADERPROC)loadExtension("glDeleteShader");
		m_shaderSource = (PFNGLSHADERSOURCEPROC)loadExtension("glShaderSource");
		m_compileShader = (PFNGLCOMPILESHADERPROC)loadExtension("glCompileShader");
		m_attachShader = (PFNGLATTACHSHADERPROC)loadExtension("glAttachShader");
		m_linkProgram = (PFNGLLINKPROGRAMPROC)loadExtension("glLinkProgram");
		m_bindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)loadExtension("glBindAttribLocation");
		m_vertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)loadExtension("glVertexAttribPointer");
		m_getUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)loadExtension("glGetUniformLocation");
		m_enableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)loadExtension("glEnableVertexAttribArray");
		m_uniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)loadExtension("glUniformMatrix4fv");
		m_uniform4f = (PFNGLUNIFORM4FPROC)loadExtension("glUniform4f");
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::glUseProgram(unsigned int _programId)
	{
		m_useProgram(_programId);
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned IVideoDriverOpenGL::glCreateProgram()
	{
		return m_createProgram();
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::glDeleteProgram	(unsigned _program)
	{
		m_deleteProgram(_program);
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned IVideoDriverOpenGL::glCreateShader	(unsigned _shaderType)
	{
		return (m_createShader)(_shaderType);
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::glDeleteShader	(unsigned _shader)
	{
		m_deleteShader(_shader);
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::glShaderSource	(unsigned _shader, int _count, const char ** _string, const int * _length)
	{
		m_shaderSource(_shader, _count, _string, _length);
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::glCompileShader (unsigned _shader)
	{
		m_compileShader(_shader);
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::glAttachShader	(unsigned _program, unsigned _shader)
	{
		m_attachShader(_program, _shader);
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::glLinkProgram	(unsigned _program)
	{
		m_linkProgram(_program);
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::glBindAttribLocation(unsigned _program, unsigned _index, const char * _name)
	{
		m_bindAttribLocation(_program, _index, _name);
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::glVertexAttribPointer(unsigned _idx, int _size, unsigned _type, bool _normalized,
													int _stride, const void * _pointer)
	{
		m_vertexAttribPointer(_idx, _size, _type, _normalized, _stride, _pointer);
	}

	//------------------------------------------------------------------------------------------------------------------
	int IVideoDriverOpenGL::glGetUniformLocation(unsigned _program, const char* _name) const
	{
		return m_getUniformLocation(_program, _name);
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::glEnableVertexAttribArray(unsigned _idx)
	{
		m_enableVertexAttribArray(_idx);
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::glUniformMatrix4fv(unsigned _location, int _count, bool _transpose, const float *_value)
	{
		m_uniformMatrix4fv(_location, _count, _transpose, _value);
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::glUniform4f(unsigned _location, float _f0, float _f1, float _f2, float _f3)
	{
		m_uniform4f(_location, _f0, _f1, _f2, _f3);
	}

}	// namespace video
}	// namespace rev
