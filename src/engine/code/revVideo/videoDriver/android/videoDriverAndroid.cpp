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

#include "revCore/codeTools/log/log.h"
#include "revCore/file/file.h"
#include "revVideo/color/color.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CVideoDriverAndroid::CVideoDriverAndroid():
		mCurShader(-1),
		mScreenWidth(640),
		mScreenHeight(800)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoDriverAndroid::beginFrame()
	{
		glViewport(0, 0, mScreenWidth, mScreenHeight);
		glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
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
	void CVideoDriverAndroid::setRealAttribBuffer	(const int _attribId, const unsigned _nComponents, const void * const _buffer)
	{
		glVertexAttribPointer(_attribId, _nComponents, GL_FLOAT, false, 0, _buffer);
		glEnableVertexAttribArray(_attribId);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoDriverAndroid::drawIndexBuffer	(const int _nIndices, const unsigned short * _indices, const bool _strip)
	{
		glDrawElements(_strip?GL_TRIANGLE_STRIP:GL_TRIANGLES, _nIndices, GL_UNSIGNED_SHORT, _indices);
	}

	//------------------------------------------------------------------------------------------------------------------
	int CVideoDriverAndroid::loadShader		(const char * _vtxName, const char * _pxlName)
	{
		// Create the program
		int program = glCreateProgram();
		// Vertex shader
		unsigned vtxShader = glCreateShader(GL_VERTEX_SHADER);
		const char * fileBuffer = bufferFromFile(_vtxName);
		glShaderSource(vtxShader, 1, &fileBuffer, 0); // Attach source
		glCompileShader(vtxShader); // Compile
		delete[] fileBuffer;
		// Pixel shader
		unsigned pxlShader = glCreateShader(GL_FRAGMENT_SHADER);
		fileBuffer = bufferFromFile(_pxlName);
		glShaderSource(pxlShader, 1, &fileBuffer, 0); // Attach source
		glCompileShader(pxlShader); // Compile
		delete[] fileBuffer;
		// Complete shader
		glAttachShader(program, vtxShader);
		glAttachShader(program, pxlShader);
		// bind attributes before linking the shader
		bindAttributes(program);
		// Link the program and load it
		glLinkProgram(program);
		return program;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoDriverAndroid::deleteShader(const int /*_shader*/)
	{
		// TODO: _solve leaks!
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoDriverAndroid::bindAttributes(int _shader)
	{
		glBindAttribLocation(_shader, eVertex, "vertex");
	}

}	// namespace video
}	// namespace rev

#endif // ANDROID

