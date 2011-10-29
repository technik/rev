////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on September 10th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// android video driver
#ifdef ANDROID

// Standard headers
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

// Engine headers
#include "videoDriverAndroid.h"

#include "revCore/codeTools/assert/assert.h"
#include "revCore/codeTools/log/log.h"
#include "revCore/file/file.h"
#include "revVideo/color/color.h"
#include "revVideo/videoDriver/shader/pxlShader.h"
#include "revVideo/videoDriver/shader/vtxShader.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CVideoDriverAndroid::CVideoDriverAndroid():
		mCurShader(-1),
		mScreenWidth(800),
		mScreenHeight(480)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoDriverAndroid::beginFrame()
	{
		glViewport(0, 0, mScreenWidth, mScreenHeight);
		glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		codeTools::revAssert(glGetError() == GL_NO_ERROR);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoDriverAndroid::endFrame()
	{
		//
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoDriverAndroid::setBackgroundColor(const CColor &_color)
	{
		glClearColor(_color.r(), _color.g(), _color.b(), _color.a());

		codeTools::revAssert(glGetError() == GL_NO_ERROR);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoDriverAndroid::setScreenSize(const unsigned int _width, const unsigned int _height)
	{
		mScreenWidth = _width;
		mScreenHeight = _height;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoDriverAndroid::setShader(const int _shader)
	{
		if(mCurShader != _shader)
		{
			mCurShader = _shader;
			glUseProgram(_shader);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	int CVideoDriverAndroid::getUniformId(const char * _name) const
	{
		return glGetUniformLocation(mCurShader, _name);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoDriverAndroid::setRealAttribBuffer	(const int _attribId, const unsigned _nComponents, const void * const _buffer)
	{
		glVertexAttribPointer(_attribId, _nComponents, GL_FLOAT, false, 0, _buffer);

		codeTools::revAssert(glGetError() == GL_NO_ERROR);

		glEnableVertexAttribArray(_attribId);

		codeTools::revAssert(glGetError() == GL_NO_ERROR);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoDriverAndroid::setUniform(int _id, const CMat4& _value)
	{
		// Given OpenGLES doesn't support transpose matrices, this must be done manually
		CMat4 transposeMtx;
		_value.transpose(transposeMtx);

		glUniformMatrix4fv(_id, 1, false, reinterpret_cast<const float*>(transposeMtx.m));
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoDriverAndroid::setUniform(int _id, const CColor& _value)
	{
		glUniform4f(_id, _value.r(), _value.g(), _value.b(), _value.a());
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoDriverAndroid::drawIndexBuffer	(const int _nIndices, const unsigned short * _indices, const bool _strip)
	{
		codeTools::revAssert(glGetError() == GL_NO_ERROR);

		glDrawElements(_strip?GL_TRIANGLE_STRIP:GL_TRIANGLES, _nIndices, GL_UNSIGNED_SHORT, _indices);

		codeTools::revAssert(glGetError() == GL_NO_ERROR);
	}

	//------------------------------------------------------------------------------------------------------------------
	int CVideoDriverAndroid::linkShader(CVtxShader * _vtx, CPxlShader * _pxl)
	{
		int program = glCreateProgram();
		glAttachShader(program, _vtx->id());
		glAttachShader(program, _pxl->id());
		bindAttributes(program);
		glLinkProgram(program);
		return program;
	}

	//------------------------------------------------------------------------------------------------------------------
	int CVideoDriverAndroid::loadVtxShader(const char * _name)
	{
		unsigned shader = glCreateShader(GL_VERTEX_SHADER);
		const char * fileBuffer = bufferFromFile(_name);
		glShaderSource(shader, 1, &fileBuffer, 0); // Attach source
		glCompileShader(shader); // Compile
		delete[] fileBuffer;
		return int(shader);
	}

	//------------------------------------------------------------------------------------------------------------------
	int CVideoDriverAndroid::loadPxlShader(const char * _name)
	{
		unsigned shader = glCreateShader(GL_FRAGMENT_SHADER);
		const char * fileBuffer = bufferFromFile(_name);
		glShaderSource(shader, 1, &fileBuffer, 0); // Attach source
		glCompileShader(shader); // Compile
		delete[] fileBuffer;
		return int(shader);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoDriverAndroid::bindAttributes(int _shader)
	{
		glBindAttribLocation(_shader, eVertex, "vertex");

		codeTools::revAssert(glGetError() == GL_NO_ERROR);

		glBindAttribLocation(_shader, eColor, "color");

		codeTools::revAssert(glGetError() == GL_NO_ERROR);
	}

}	// namespace video
}	// namespace rev

#endif // ANDROID

