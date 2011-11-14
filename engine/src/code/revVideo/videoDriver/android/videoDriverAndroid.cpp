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
#include "revVideo/texture/texture.h"
#include "revVideo/videoDriver/shader/pxlShader.h"
#include "revVideo/videoDriver/shader/vtxShader.h"

#define GL_SHADER_VERBOSE
//#define GL_VERBOSE
#ifdef GL_VERBOSE
#define GL_LOG( ... ) LOG_ANDROID( __VA_ARGS__ )
#else
#define GL_LOG( ... )
#endif

#define ASSERT_GL( ... ) \
{int errorCode = glGetError();\
if(GL_NO_ERROR != errorCode) \
{\
	switch(errorCode)\
	{\
		case GL_INVALID_OPERATION:\
		{\
			LOG_ANDROID("GL_INVALID_OPERATION");\
			break;\
		}\
		case GL_INVALID_VALUE:\
		{\
			LOG_ANDROID("GL_INVALID_VALUE");\
			break;\
		}\
		case GL_INVALID_ENUM:\
		{\
			LOG_ANDROID("GL_INVALID_ENUM");\
			break;\
		}\
		default:\
		{\
			LOG_ANDROID("GL_OTHER");\
			break;\
		}\
	}\
	LOG_ANDROID( __VA_ARGS__ );\
	codeTools::revAssert(false);\
}}

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

		ASSERT_GL("glViewport");
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoDriverAndroid::endFrame()
	{
		//
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoDriverAndroid::setBackgroundColor(const CColor &_color)
	{
		LOG_ANDROID("setBackgroundColor");
		glClearColor(_color.r(), _color.g(), _color.b(), _color.a());

		ASSERT_GL("glClearColor");
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
			GL_LOG("glUseProgram %d", _shader);
#ifdef GL_VERBOSE
			char log[512];
			GLsizei logLength;
			glGetProgramInfoLog(_shader,
								512,
								&logLength,
								log);
			LOG_ANDROID("program log:%s", log);
#endif
			mCurShader = _shader;
			glUseProgram(_shader);
			ASSERT_GL("glUseProgram %d", _shader);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	int CVideoDriverAndroid::getUniformId(const char * _name) const
	{
		int ret = glGetUniformLocation(mCurShader, _name);

		GL_LOG("%d = getUniformId %s", ret, _name);
		ASSERT_GL("glGetUniformLocation curShader=%d, name=\"%s\"", mCurShader, _name);

		return ret;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoDriverAndroid::setRealAttribBuffer	(const int _attribId, const unsigned _nComponents, const void * const _buffer)
	{
		GL_LOG("setRealAttribBuffer id=%d", _attribId);
		glVertexAttribPointer(_attribId, _nComponents, GL_FLOAT, false, 0, _buffer);

		ASSERT_GL("glVertexAttribPointer attrib=%d, components=\%d", _attribId, _nComponents);

		glEnableVertexAttribArray(_attribId);

		ASSERT_GL("glEnableVertexAttribArray attrib=%d", _attribId);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoDriverAndroid::setUniform(int _id, float _f)
	{
		glUniform1f(_id, _f);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoDriverAndroid::setUniform(int _id, const CMat4& _value)
	{
		GL_LOG("setUnifrom(CMat4) id=%d", _id);
		// Given OpenGLES doesn't support transpose matrices, this must be done manually
		CMat4 transposeMtx;
		_value.transpose(transposeMtx);

		glUniformMatrix4fv(_id, 1, false, reinterpret_cast<const float*>(transposeMtx.m));

		ASSERT_GL("glUniformMatrix4fv attrib=%d", _id);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoDriverAndroid::setUniform(int _id, int _slot, const CTexture* _texture)
	{
		if(0 == _slot)
		{
			glActiveTexture(GL_TEXTURE0);
			glUniform1i(_id, 0);
		}
		else codeTools::revAssert(false); // Unimplemented: Check how many slots are available

		glBindTexture(GL_TEXTURE_2D, _texture->id());
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoDriverAndroid::setUniform(int _id, const CColor& _value)
	{
		GL_LOG("setUniform(CColor) id=%d", _id);
		glUniform4f(_id, _value.r(), _value.g(), _value.b(), _value.a());

		ASSERT_GL("glUniform4f attrib=%d", _id);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoDriverAndroid::drawIndexBuffer	(const int _nIndices, const unsigned short * _indices, const bool _strip)
	{
		GL_LOG("drawIndexBuffer nIndixes=%d", _nIndices);
		glDrawElements(_strip?GL_TRIANGLE_STRIP:GL_TRIANGLES, _nIndices, GL_UNSIGNED_SHORT, _indices);

		ASSERT_GL("glDrawElements nIndices=%d", _nIndices);
	}

	//------------------------------------------------------------------------------------------------------------------
	int CVideoDriverAndroid::linkShader(CVtxShader * _vtx, CPxlShader * _pxl)
	{
		int program = glCreateProgram();
		glAttachShader(program, _vtx->id());ASSERT_GL("glAttachShader program=%d, shader=%d", program, _vtx->id());
		glAttachShader(program, _pxl->id());ASSERT_GL("glAttachShader program=%d, shader=%d", program, _pxl->id());
		bindAttributes(program);
		glLinkProgram(program);
		GL_LOG("%d = link shader, %d %d", program, _vtx->id(), _pxl->id());
#if defined(GL_VERBOSE) || defined(GL_SHADER_VERBOSE)
			char log[512];
			GLsizei logLength;
			glGetProgramInfoLog(program,
								512,
								&logLength,
								log);
			LOG_ANDROID("program log:%s", log);
#endif
		ASSERT_GL("glLinkProgram program=%d", program);
		return program;
	}

	//------------------------------------------------------------------------------------------------------------------
	int CVideoDriverAndroid::loadVtxShader(const char * _name)
	{
		unsigned shader = glCreateShader(GL_VERTEX_SHADER);
		char * fileBuffer = bufferFromFile(_name);
		const char * constFileBuffer = fileBuffer;
		// -------- HACKY CODE --------
		// This forces correct file ending in shaders
		int bufferSize = 0;
		while(0 != fileBuffer[bufferSize])
		{
			++bufferSize;
		}
		for(int i = 0; i < bufferSize - 2; ++i)
		{
			if((fileBuffer[i] == '/') && (fileBuffer[i+1] == '/') && (fileBuffer[i+2] == '#'))
			fileBuffer[i] = 0;
		}
		// ------ HACKY CODE END ------
		glShaderSource(shader, 1, &constFileBuffer, 0); // Attach source
		glCompileShader(shader); // Compile

		GL_LOG("%d = loadVtxShader \"%s\"", int(shader), _name);
#if defined(GL_VERBOSE) || defined(GL_SHADER_VERBOSE)
			LOG_ANDROID("shaderCode:\n%s",fileBuffer);
			char log[512];
			GLsizei logLength;
			glGetShaderInfoLog(shader,
								512,
								&logLength,
								log);
			LOG_ANDROID("shader log:%s", log);
#endif
		ASSERT_GL("glCompileShader shader=%d", shader);
		delete[] fileBuffer;
		return int(shader);
	}

	//------------------------------------------------------------------------------------------------------------------
	int CVideoDriverAndroid::loadPxlShader(const char * _name)
	{
		unsigned shader = glCreateShader(GL_FRAGMENT_SHADER);
		char * fileBuffer = bufferFromFile(_name);
		const char * constFileBuffer = fileBuffer;
		// -------- HACKY CODE --------
		// This forces correct file ending in shaders
		int bufferSize = 0;
		while(0 != fileBuffer[bufferSize])
		{
			++bufferSize;
		}
		for(int i = 0; i < bufferSize - 2; ++i)
		{
			if((fileBuffer[i] == '/') && (fileBuffer[i+1] == '/') && (fileBuffer[i+2] == '#'))
			fileBuffer[i] = 0;
		}
		// ------ HACKY CODE END ------
		glShaderSource(shader, 1, &constFileBuffer, 0); // Attach source
		glCompileShader(shader); // Compile

		GL_LOG("%d = loadPxlShader \"%s\"", int(shader), _name);
#if defined(GL_VERBOSE) || defined(GL_SHADER_VERBOSE)
			LOG_ANDROID("shaderCode:\n%s",fileBuffer);
			char log[512];
			GLsizei logLength;
			glGetShaderInfoLog(shader,
								512,
								&logLength,
								log);
			LOG_ANDROID("shader log:%s", log);
#endif
		delete[] fileBuffer;
		return int(shader);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoDriverAndroid::bindAttributes(int _shader)
	{
		GL_LOG("binfAttributes shader=%d", _shader);
		glBindAttribLocation(_shader, eVertex, "vertex");
		glBindAttribLocation(_shader, eTexCoord, "texCoord");

		ASSERT_GL("glBindAttribLocation shader=%d, attrib=%s", _shader, "vertex");
	}

}	// namespace video
}	// namespace rev

#endif // ANDROID

