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

#include <revCore/file/file.h>
#include <revVideo/types/color/color.h>
#include <revVideo/types/shader/pixel/openGL21/pxlShaderOpenGL21.h>
#include <revVideo/types/shader/openGL21/shaderOpenGL21.h>
#include <revVideo/types/shader/vertex/openGL21/vtxShaderOpenGL21.h>

using namespace rev::math;

// loadExtension must be a macro in order to allow extensions to be loaded from the constructor. If it was a virtual function,
// you would not be able to call it during construction, and it would require extra complexity to get this system work properly.
// Note: This has proven the wrong way to do things. As usual, there is a good reason not to allow virtuals here. Virtuals may
// depend. So...TODO: Make loadExtensions into a virtual function
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
		:mCurShader(nullptr)
		,mCurVtxShader(nullptr)
	{
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
	void Driver3dOpenGL21::setShader(const Shader * _shd)
	{
		if(_shd == mCurShader) {
			return; 
		} else mCurShader = _shd;
		const ShaderOpenGL21 * shader = static_cast<const ShaderOpenGL21*>(_shd);

		// Set active attributes
		const VtxShaderOpenGL21* vtx21 = shader->vtxShader();
		if(mCurVtxShader != vtx21) {
			if(nullptr != mCurVtxShader) { // Disable old attributes
				const std::map<std::string, unsigned>& oldAttributes = mCurVtxShader->attributes();
				for(auto iter = oldAttributes.begin(); iter != oldAttributes.end(); ++iter) {
					glDisableVertexAttribArray(iter->second);
				}
			}
			// Enable new attributes
			const std::map<std::string, unsigned>& newAttributes = vtx21->attributes();
			for(auto iter = newAttributes.begin(); iter != newAttributes.end(); ++iter){
				glEnableVertexAttribArray(iter->second);
			}
			mCurVtxShader = vtx21; // Update cache
		}

		glUseProgram(shader->id());
	}

	//------------------------------------------------------------------------------------------------------------------
	PxlShader * Driver3dOpenGL21::createPxlShader(const char * _fileName)
	{
		File * shaderFile = File::open(_fileName, false);
		revAssert(nullptr != shaderFile, "Error: Couldn't open shader file");
		unsigned shaderId = glCreateShader(GL_FRAGMENT_SHADER);
		const char * code[1];
		code[0] = shaderFile->bufferAsText();
		glShaderSource(shaderId, 1, code, 0); // Attach source
		glCompileShader(shaderId); // Compile
		if(!detectShaderError(shaderId, _fileName))
			return new PxlShaderOpenGL21(shaderId);
		else
		{
			glDeleteShader(shaderId);
			return nullptr;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	Shader * Driver3dOpenGL21::createShader(VtxShader* _vtx, PxlShader * _pxl)
	{
		// Cast to the openGL version of the shaders
		if((nullptr == _vtx) || (nullptr == _pxl))
			return nullptr;
		const VtxShaderOpenGL21 * vtx21 = static_cast<const VtxShaderOpenGL21*>(_vtx);
		const PxlShaderOpenGL21 * pxl21 = static_cast<const PxlShaderOpenGL21*>(_pxl);

		// Create an OpenGL program and attach the shaders
		GLuint program = glCreateProgram();
		glAttachShader(program, vtx21->id());
		glAttachShader(program, pxl21->id());

		// Bind attributes
		const std::map<std::string, unsigned>& attributeMap = vtx21->attributes();
		for(auto iter = attributeMap.begin(); iter != attributeMap.end(); ++iter)
		{
			glBindAttribLocation(program, iter->second, iter->first.c_str());
		}

		// Link the program
		glLinkProgram(program);

		return new ShaderOpenGL21(program, vtx21);
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
	VtxShader * Driver3dOpenGL21::createVtxShader(const char * _fileName)
	{
		File * shaderFile = File::open(_fileName, false);
		revAssert(nullptr != shaderFile, "Error: Couldn't open shader file");
		unsigned shaderId = glCreateShader(GL_VERTEX_SHADER);
		const char * code[1];
		code[0] = shaderFile->bufferAsText();
		glShaderSource(shaderId, 1, code, 0); // Attach source
		glCompileShader(shaderId); // Compile
		if(!detectShaderError(shaderId, _fileName))
			return new VtxShaderOpenGL21(shaderId);
		else
		{
			glDeleteShader(shaderId);
			return nullptr;
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