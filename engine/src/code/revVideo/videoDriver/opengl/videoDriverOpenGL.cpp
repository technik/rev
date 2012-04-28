////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on September 11th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OpenGL based video driver

// Engine headers
#include "videoDriverOpenGL.h"

#include <revCore/codeTools/assert/assert.h>
#include <revCore/codeTools/log/log.h>
#include <revCore/file/file.h>
#include <revCore/math/matrix.h>
#include <revCore/string.h>
#include <revVideo/color/color.h>
#include <revVideo/texture/texture.h>
#include <revVideo/videoDriver/shader/pxlShader.h>
#include <revVideo/videoDriver/shader/vtxShader.h>

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
	IVideoDriverOpenGL::IVideoDriverOpenGL()
		// Internal state and caches
		:mCurShader(-1)
		,mMVPUniformId(-1)
		,mScreenWidth(800)
		,mScreenHeight(480)
		,m0Idx(0)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::setModelViewMatrix(const CMat34& _mv)
	{
		mModelView = _mv;
		m0Idx = mVertexCache.size();
		if((m0Idx + mNVertices) > 0xffff)
		{
			flushGeometryCache();
			m0Idx = 0;
		}
		// Copy geometry data
		for(unsigned i = 0; i < mNVertices; ++i)
		{
			mVertexCache.push_back(mModelView * mVertexBuffer[i]);
			mNormalCache.push_back(mNormalBuffer[i]);
			mTexCoordCache.push_back(mTexCoordBuffer[i]);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::setProjMatrix(const CMat4& _proj)
	{
		setUniform(mMVPUniformId, _proj);
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::setShader(const int _shader)
	{
		revAssert(_shader >= 0);
		if(_shader != mCurShader) // Check cache
		{
			flushGeometryCache();
			mCurShader = _shader;
			glUseProgram(unsigned(mCurShader));
			mMVPUniformId = getUniformId("modelViewProj");
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	int IVideoDriverOpenGL::getUniformId(const char * _name) const
	{
		return glGetUniformLocation(mCurShader, _name);
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::setRealAttribBuffer(const int _attribId
												,unsigned _nElements
												,const unsigned _nComponents
												,const void * const _buffer)
	{
		// Assert incomming data is valid
		revAssert((_nComponents > 0) && (_nComponents < 5)); // [1,4] reals per buffer element
		revAssert(_attribId >= 0); // Valid id
		revAssert(0 != _buffer); // Non-null buffer

		// Process data
		if(_attribId == eVertex)
		{
			mNVertices = _nElements;
			mVertexBuffer = reinterpret_cast<const CVec3*>(_buffer);
		}
		else if(_attribId == eNormal)
		{
			mNormalBuffer = reinterpret_cast<const CVec3*>(_buffer);
		}
		else if(_attribId == eTexCoord)
		{
			mTexCoordBuffer = reinterpret_cast<const CVec2*>(_buffer);
		}
		else
		{
			flushGeometryCache();
			glVertexAttribPointer(unsigned(_attribId), _nComponents, GL_FLOAT, false, 0, _buffer);
			glEnableVertexAttribArray(_attribId);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::setUniform(int _id, float _value)
	{
		if(_id > 0)
		{
			flushGeometryCache();
			glUniform1f(_id, _value);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::setUniform(int _id, const CMat4& _value)
	{
		if(_id >= 0) // Valid uniform
		{
			flushGeometryCache();
			m_uniformMatrix4fv(_id, 1, true, reinterpret_cast<const float*>(_value.m));
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::setUniform(int _id, const CColor& _value)
	{
		if(_id >= 0) // Valid uniform
		{
			flushGeometryCache();
			glUniform4f(_id, _value.r(), _value.g(), _value.b(), _value.a());
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::setUniform(int _id, const CVec3& _value)
	{
		if(_id >= 0) // Valid uniform
		{
			flushGeometryCache();
			glUniform3f(_id, _value.x, _value.y, _value.z);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::setUniform(int _id, int _slot, const CTexture * _value)
	{
		revAssert(0 == _slot, "Only one texture slot supported");
	
		flushGeometryCache();
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(_id, 0);
		
		glBindTexture(GL_TEXTURE_2D, _value->id());
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::drawIndexBuffer(const int _nIndices, const unsigned short int * _indices,
		EPrimitiveType _primitive)
	{
		switch(_primitive)
		{
		case eTriangle:
			for(int i = 0; i < _nIndices; ++i)
			{
				mTriangleCache.push_back(_indices[i] + (u16)m0Idx);
			}
			// glDrawElements(GL_TRIANGLES, _nIndices, GL_UNSIGNED_SHORT, _indices);
			break;
		case eTriStrip:
			for(int i = 0; i < _nIndices; ++i)
			{
				mTriStripCache.push_back(_indices[i] + (u16)m0Idx);
			}
			//glDrawElements(GL_TRIANGLE_STRIP, _nIndices, GL_UNSIGNED_SHORT, _indices);
			break;
		case eLine:
			for(int i = 0; i < _nIndices; ++i)
			{
				mLineCache.push_back(_indices[i] + (u16)m0Idx);
			}
			//glDrawElements(GL_LINES, _nIndices, GL_UNSIGNED_SHORT, _indices);
			break;
		case eLineStrip:
			for(int i = 0; i < _nIndices; ++i)
			{
				mLineStripCache.push_back(_indices[i] + (u16)m0Idx);
			}
			//glDrawElements(GL_LINE_STRIP, _nIndices, GL_UNSIGNED_SHORT, _indices);
			break;
		default:
			revAssert(false);	// Unsupported
		}
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
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_BLEND);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_POLYGON_SMOOTH);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_CULL_FACE);
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::endFrame()
	{
		flushGeometryCache();
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::flushGeometryCache()
	{
		this->glVertexAttribPointer(unsigned(eVertex), 3, GL_FLOAT, false, 0, mVertexCache.data());
		this->glVertexAttribPointer(unsigned(eNormal), 3, GL_FLOAT, false, 0, mNormalCache.data());
		this->glVertexAttribPointer(unsigned(eTexCoord), 2, GL_FLOAT, false, 0, mTexCoordCache.data());
		glEnableVertexAttribArray(eVertex);
		glEnableVertexAttribArray(eNormal);
		glEnableVertexAttribArray(eTexCoord);
		// Draw triangles
		if(!mTriangleCache.empty())
		{
			glDrawElements(GL_TRIANGLES, mTriangleCache.size(), GL_UNSIGNED_SHORT, mTriangleCache.data());
			mTriangleCache.clear();
		}
		// Draw triangles
		
		if(!mTriStripCache.empty())
		{
			glDrawElements(GL_TRIANGLE_STRIP, mTriStripCache.size(), GL_UNSIGNED_SHORT, mTriStripCache.data());
			mTriStripCache.clear();
		}
		// Draw triangles
		
		if(!mLineCache.empty())
		{
			glDrawElements(GL_LINES, mLineCache.size(), GL_UNSIGNED_SHORT, mLineCache.data());
			mLineCache.clear();
		}
		// Draw triangles
		if(!mLineStripCache.empty())
		{
			glDrawElements(GL_LINE_STRIP, mLineStripCache.size(), GL_UNSIGNED_SHORT, mLineStripCache.data());
			mLineStripCache.clear();
		}
		mVertexCache.clear();
		mNormalCache.clear();
		mTexCoordCache.clear();
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
	void IVideoDriverOpenGL::destroyShader(int _id)
	{
		glDeleteProgram(_id);
	}

	//------------------------------------------------------------------------------------------------------------------
	int IVideoDriverOpenGL::loadVtxShader(const char * _name)
	{
		unsigned shader = glCreateShader(GL_VERTEX_SHADER);
		CFile file(_name);
		const char * fileBuffer = reinterpret_cast<const char*>(file.buffer());
		glShaderSource(shader, 1, &fileBuffer, 0); // Attach source
		glCompileShader(shader); // Compile
		if(!detectShaderError(shader))
			return int(shader);
		else
		{
			glDeleteShader(shader);
			return -1;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	int IVideoDriverOpenGL::loadPxlShader(const char * _name)
	{
		unsigned shader = glCreateShader(GL_FRAGMENT_SHADER);
		CFile file(_name);
		const char * fileBuffer = reinterpret_cast<const char*>(file.buffer());
		glShaderSource(shader, 1, &fileBuffer, 0); // Attach source
		glCompileShader(shader); // Compile
		if(!detectShaderError(shader))
			return int(shader);
		else
		{
			glDeleteShader(shader);
			return -1;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::releaseShader(int _id)
	{
		glDeleteShader(_id);
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::bindAttributes(int _shader)
	{
		glBindAttribLocation(_shader, eVertex, "vertex");
		glBindAttribLocation(_shader, eNormal, "normal");
		glBindAttribLocation(_shader, eTexCoord, "uv0");
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::loadOpenGLExtensions()
	{
		m_useProgram = (PFNGLUSEPROGRAMPROC)loadExtension("glUseProgram");
		m_createProgram = (PFNGLCREATEPROGRAMPROC)loadExtension("glCreateProgram");
		m_deleteProgram = (PFNGLDELETEPROGRAMPROC)loadExtension("glDeleteProgram");
		m_createShader = (PFNGLCREATESHADERPROC)loadExtension("glCreateShader");
		m_deleteShader = (PFNGLDELETESHADERPROC)loadExtension("glDeleteShader");
		m_shaderSource = (PFNGLSHADERSOURCEPROC)loadExtension("glShaderSource");
		m_compileShader = (PFNGLCOMPILESHADERPROC)loadExtension("glCompileShader");
		m_getShaderiv = (PFNGLGETSHADERIVPROC)loadExtension("glGetShaderiv");
		m_getShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)loadExtension("glGetShaderInfoLog");
		m_attachShader = (PFNGLATTACHSHADERPROC)loadExtension("glAttachShader");
		m_linkProgram = (PFNGLLINKPROGRAMPROC)loadExtension("glLinkProgram");
		m_bindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)loadExtension("glBindAttribLocation");
		m_vertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)loadExtension("glVertexAttribPointer");
		m_getUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)loadExtension("glGetUniformLocation");
		m_enableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)loadExtension("glEnableVertexAttribArray");
		m_uniform1f = (PFNGLUNIFORM1FPROC)loadExtension("glUniform1f");
		m_uniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)loadExtension("glUniformMatrix4fv");
		m_uniform3f = (PFNGLUNIFORM3FPROC)loadExtension("glUniform3f");
		m_uniform4f = (PFNGLUNIFORM4FPROC)loadExtension("glUniform4f");
		m_uniform1i = (PFNGLUNIFORM1IPROC)loadExtension("glUniform1i");
		m_activeTexture = (PFNGLACTIVETEXTUREPROC)loadExtension("glActiveTexture");
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
	void IVideoDriverOpenGL::glUniform1f(unsigned _location, float _f)
	{
		m_uniform1f(_location, _f);
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::glUniform3f(unsigned _location, float _f0, float _f1, float _f2)
	{
		m_uniform3f(_location, _f0, _f1, _f2);
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::glUniform4f(unsigned _location, float _f0, float _f1, float _f2, float _f3)
	{
		m_uniform4f(_location, _f0, _f1, _f2, _f3);
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::glUniform1i(unsigned _location, int _i)
	{
		m_uniform1i(_location, _i);
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::glActiveTexture(GLenum _texture)
	{
		m_activeTexture(_texture);
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::glGetShaderiv(unsigned _shader, unsigned _pName, int * _params) const
	{
		m_getShaderiv(_shader, _pName, _params);
	}

	//------------------------------------------------------------------------------------------------------------------
	void IVideoDriverOpenGL::glGetShaderInfoLog(unsigned _shader, unsigned _maxLength, int * _length, char * _buffer) const
	{
		m_getShaderInfoLog(_shader,_maxLength,_length,_buffer);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool IVideoDriverOpenGL::detectShaderError(unsigned _shader) const
	{
		int status;
		glGetShaderiv(_shader, GL_COMPILE_STATUS, &status);
		if(status == GL_TRUE)
		{
			return false;
		}
		else
		{
			char buffer[1024];
			int len;
			glGetShaderInfoLog(_shader, 1024, &len, buffer);
			buffer[len] = '\0';
			revLogN("Error compiling shader", eError);
			revAssert(false, buffer);
			return true;
		}
	}

}	// namespace video
}	// namespace rev
