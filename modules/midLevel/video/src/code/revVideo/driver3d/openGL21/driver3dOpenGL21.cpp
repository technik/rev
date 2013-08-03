//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 10th, 2012
//----------------------------------------------------------------------------------------------------------------------
// OpenGL 2.1 based 3d drivers' interface

#include "driver3dOpenGL21.h"

#ifdef WIN32
#include <Windows.h>
#include <gl/GL.h>
#endif // WIN32
#ifdef __linux__
#include <GL/glx.h>º
#endif // __linux__

#include "glext.h"

#include <revCore/codeTools/assert/assert.h>
#include <revPlatform/fileSystem/fileSystem.h>
#include <revVideo/types/color/color.h>

using namespace rev::math;

// loadExtension must be a macro in order to allow extensions to be loaded from the constructor. If it was a virtual function,
// you would not be able to call it during construction, and it would require extra complexity to get this system work properly.
#ifdef WIN32
#define loadExtension( a ) wglGetProcAddress( a )
#endif // WIN32
#ifdef __linux__
#define loadExtension( a ) glXGetProcAddress( (GLubyte*)a )
#endif // __linux__

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	Driver3dOpenGL21::Driver3dOpenGL21()
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::init()
	{
		loadShader();
		setShader();
		auto shaderReload = [this](const char*){
			glDeleteProgram(mProgram);
			loadShader();
			setShader();
		};
		platform::FileSystem::get()->onFileChanged("test.vtx") += shaderReload;
		platform::FileSystem::get()->onFileChanged("test.pxl") += shaderReload;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::setViewport(const Vec2i& _position, const Vec2u& _size)
	{
		glViewport(GLint(_position.x),
					GLint(_position.y),
					GLsizei(_size.x),
					GLsizei(_size.y));
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::clearZBuffer()
	{
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::clearColorBuffer()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::setClearColor(const Color& _clr)
	{
		glClearColor(GLclampf(_clr.r), GLclampf(_clr.g), GLclampf(_clr.b), GLclampf(_clr.a));
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::setZCompare(bool _enable)
	{
		if(_enable)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::flush()
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::finishFrame()
	{
		flush();
		swapBuffers();
	}

	//------------------------------------------------------------------------------------------------------------------
	int Driver3dOpenGL21::getUniformLocation(const char* _uniform)
	{
		return glGetUniformLocation(mProgram, _uniform);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::drawIndexBuffer(int _nIndices, unsigned short * _indices, Driver3d::EPrimitiveType _primitive)
	{
		switch(_primitive)
		{
		case EPrimitiveType::lines:
			glDrawElements(GL_LINES, _nIndices, GL_UNSIGNED_SHORT, _indices);
			break;
		case EPrimitiveType::triangles:
			glDrawElements(GL_TRIANGLES, _nIndices, GL_UNSIGNED_SHORT, _indices);
			break;
		case EPrimitiveType::triStrip:
			glDrawElements(GL_TRIANGLE_STRIP, _nIndices, GL_UNSIGNED_SHORT, _indices);
			break;
		default:
			revAssert(false, "Error: Trying to render an unimplemented primitive type");
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::setAttribBuffer(unsigned _attribId, unsigned _nElements, const float* _buffer)
	{
		REV_UNUSED_PARAM(_nElements);
		glVertexAttribPointer(_attribId, 1, GL_FLOAT, GL_FALSE, 0, _buffer);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::setAttribBuffer(unsigned _attribId, unsigned _nElements, const uint16_t* _buffer)
	{
		REV_UNUSED_PARAM(_nElements);
		glVertexAttribPointer(_attribId, 1, GL_UNSIGNED_SHORT, GL_FALSE, 0, _buffer);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::setAttribBuffer(unsigned _attribId, unsigned _nElements, const int16_t* _buffer)
	{
		REV_UNUSED_PARAM(_nElements);
		glVertexAttribPointer(_attribId, 1, GL_SHORT, GL_FALSE, 0, _buffer);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::setAttribBuffer(unsigned _attribId, unsigned _nElements, const math::Vec2f* _buffer)
	{
		REV_UNUSED_PARAM(_nElements);
		glVertexAttribPointer(_attribId, 2, GL_FLOAT, GL_FALSE, 0, _buffer);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::setAttribBuffer(unsigned _attribId, unsigned _nElements, const math::Vec3f* _buffer)
	{
		REV_UNUSED_PARAM(_nElements);
		glVertexAttribPointer(_attribId, 3, GL_FLOAT, GL_FALSE, 0, _buffer);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::setUniform(int _uniformId, float _value)
	{
		glUniform1f(_uniformId, _value);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::setUniform(int _uniformId, const math::Vec2f& _value)
	{
		glUniform2f(_uniformId, _value.x, _value.y);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::setUniform(int _uniformId, const math::Vec3f& _value)
	{
		glUniform3f(_uniformId, _value.x, _value.y, _value.z);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::setUniform(int _uniformId, const math::Mat44f& _value)
	{
		glUniformMatrix4fv(_uniformId, 1, true, &_value[0][0]);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::setUniform(int _uniformId, const Color& _value)
	{
		glUniform4f(_uniformId, _value.r, _value.g, _value.b, _value.a);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::loadShader()
	{
		// Load vertex shader
		unsigned vtxShaderId = glCreateShader(GL_VERTEX_SHADER);
		const char * vtxCode[1];
		vtxCode[0] = (const char*)platform::FileSystem::get()->getFileAsBuffer("test.vtx").mBuffer;
		revAssert(nullptr != vtxCode[0], "Error: Couldn't open shader file");
		glShaderSource(vtxShaderId, 1, vtxCode, 0); // Attach source
		glCompileShader(vtxShaderId); // Compile
		if(detectShaderError(vtxShaderId, "test.vtx")) {
			glDeleteShader(vtxShaderId);
			return;
		}
		
		// Load pixel shader
		unsigned pxlShaderId = glCreateShader(GL_FRAGMENT_SHADER);
		const char * pxlCode[1];
		pxlCode[0] = (const char*)platform::FileSystem::get()->getFileAsBuffer("test.pxl").mBuffer;
		revAssert(nullptr != pxlCode[0], "Error: Couldn't open shader file");
		glShaderSource(pxlShaderId, 1, pxlCode, 0); // Attach source
		glCompileShader(pxlShaderId); // Compile
		if(detectShaderError(pxlShaderId, "test.pxl")) {
			glDeleteShader(pxlShaderId);
			return;
		}

		// Create an OpenGL program and attach the shaders
		mProgram = glCreateProgram();
		glAttachShader(mProgram, vtxShaderId);
		glAttachShader(mProgram, pxlShaderId);
		// Bind attributes
		glBindAttribLocation(mProgram, 0, "vertex");
		glBindAttribLocation(mProgram, 1, "normal");
		glBindAttribLocation(mProgram, 2, "uv");

		glLinkProgram(mProgram);

		glDeleteShader(vtxShaderId);
		glDeleteShader(pxlShaderId);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::setShader()
	{
		// Set active attributes
		glUseProgram(mProgram);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::loadOpenGLExtensions()
	{
		mAttachShader				= (PFNGLATTACHSHADERPROC)				loadExtension("glAttachShader");
		mBindAttribLocation			= (PFNGLBINDATTRIBLOCATIONPROC)			loadExtension("glBindAttribLocation");
		mCompileShader				= (PFNGLCOMPILESHADERPROC)				loadExtension("glCompileShader");
		mCreateProgram				= (PFNGLCREATEPROGRAMPROC)				loadExtension("glCreateProgram");
		mCreateShader				= (PFNGLCREATESHADERPROC)				loadExtension("glCreateShader");
		mDeleteProgram				= (PFNGLDELETEPROGRAMPROC)				loadExtension("glDeleteProgram");
		mDeleteShader				= (PFNGLDELETESHADERPROC)				loadExtension("glDeleteShader");
		mDisableVertexAttribArray	= (PFNGLDISABLEVERTEXATTRIBARRAYPROC)	loadExtension("glDisableVertexAttribArray");
		mEnableVertexAttribArray	= (PFNGLENABLEVERTEXATTRIBARRAYPROC)	loadExtension("glEnableVertexAttribArray");
		mGetUniformLocation			= (PFNGLGETUNIFORMLOCATIONPROC)			loadExtension("glGetUniformLocation");
		mGetShaderInfoLog			= (PFNGLGETSHADERINFOLOGPROC)			loadExtension("glGetShaderInfoLog");
		mGetShaderiv				= (PFNGLGETSHADERIVPROC)				loadExtension("glGetShaderiv");
		mLinkProgram				= (PFNGLLINKPROGRAMPROC)				loadExtension("glLinkProgram");
		mShaderSource				= (PFNGLSHADERSOURCEPROC)				loadExtension("glShaderSource");
		mUniform1f					= (PFNGLUNIFORM1FPROC)					loadExtension("glUniform1f");
		mUniform2f					= (PFNGLUNIFORM2FPROC)					loadExtension("glUniform2f");
		mUniform3f					= (PFNGLUNIFORM3FPROC)					loadExtension("glUniform3f");
		mUniform4f					= (PFNGLUNIFORM4FPROC)					loadExtension("glUniform4f");
		mUniformMatrix4fv			= (PFNGLUNIFORMMATRIX4FVPROC)			loadExtension("glUniformMatrix4fv");
		mUseProgram					= (PFNGLUSEPROGRAMPROC)					loadExtension("glUseProgram");
		mVertexAttribPointer		= (PFNGLVERTEXATTRIBPOINTERPROC)		loadExtension("glVertexAttribPointer");
	};

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::glBindAttribLocation(GLuint _program, GLuint _index, const GLchar* _name)
	{
		mBindAttribLocation(_program, _index, _name);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::glDisableVertexAttribArray(GLuint _idx)
	{
		mDisableVertexAttribArray(_idx);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::glEnableVertexAttribArray(GLuint _idx)
	{
		mEnableVertexAttribArray(_idx);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::glShaderSource(GLuint _shader, GLsizei _count, const GLchar ** _string, const GLint * _length)
	{
		mShaderSource(_shader, _count, _string, _length);
	}

	//------------------------------------------------------------------------------------------------------------------
	GLint Driver3dOpenGL21::glGetUniformLocation(GLuint _program, const GLchar* _uniform)
	{
		return mGetUniformLocation(_program, _uniform);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::glGetShaderInfoLog(GLuint _shader, GLsizei _maxLen, GLsizei* _length, GLchar* _infoLog)
	{
		mGetShaderInfoLog(_shader,_maxLen,_length,_infoLog);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::glGetShaderiv(GLuint _shader, GLenum _paramName, GLint* _params)
	{
		mGetShaderiv(_shader,_paramName,_params);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Driver3dOpenGL21::detectShaderError(unsigned _shaderId, const char * _shaderName)
	{
		int status;
		glGetShaderiv(_shaderId, GL_COMPILE_STATUS, &status);
		if(status == GL_TRUE)
		{
			return false;
		}
		else
		{
			char buffer[1024];
			int len;
			glGetShaderInfoLog(_shaderId, 1024, &len, buffer);
			buffer[len] = '\0';
			revLog() << "Error compiling shader \"" << _shaderName << "\"\n"
				<< buffer << "\n";
			revLog().flush();
			return true;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::glUniform1f(GLint _uniformId, GLfloat _value)
	{
		mUniform1f(_uniformId, _value);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::glUniform2f(GLint _uniformId, GLfloat _f0, GLfloat _f1)
	{
		mUniform2f(_uniformId, _f0, _f1);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::glUniform3f(GLint _uniformId, GLfloat _f0, GLfloat _f1, GLfloat _f2)
	{
		mUniform3f(_uniformId, _f0, _f1, _f2);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::glUniform4f(GLint _uniformId, GLfloat _f0, GLfloat _f1, GLfloat _f2, GLfloat _f3)
	{
		mUniform4f(_uniformId, _f0, _f1, _f2, _f3);
	}
	
	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::glUniformMatrix4fv(GLint _uniform, GLsizei _count, GLboolean _transpose, const GLfloat* _value)
	{
		mUniformMatrix4fv(_uniform, _count, _transpose, _value);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Driver3dOpenGL21::glVertexAttribPointer(GLuint _idx, GLint _size,
		GLenum _type, bool _normalized, GLsizei _stride, const GLvoid * _pointer)
	{
		mVertexAttribPointer(_idx, _size, _type, _normalized, _stride, _pointer);
	}

}	// namespace video
}	// namespace rev