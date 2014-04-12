//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/06
//----------------------------------------------------------------------------------------------------------------------
// Interface with OpenGL
#include "openGLDriver.h"

#include <gl/GL.h>
#include "glew.h"

#include <video/basicTypes/color.h>
#include <video/graphics/shader/openGL/openGLShader.h>

using namespace rev::core;

namespace rev {
	namespace video {
		//--------------------------------------------------------------------------------------------------------------
		OpenGLDriver::OpenGLDriver(const Window* _window) : OpenGLDriverBase(_window) {
			Shader::manager()->setCreator(
				[](const string& _name) -> Shader* {
					return new OpenGLShader(_name);
				});
		}

		//--------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::setViewport(const math::Vec2i& _position, const math::Vec2u& _size) {
			glViewport(_position.x, _position.y, _size.x, _size.y);
		}

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::clearZBuffer()
		{
			glClear(GL_DEPTH_BUFFER_BIT);
		}

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::clearColorBuffer()
		{
			glClear(GL_COLOR_BUFFER_BIT);
		}

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::setClearColor(const Color& _clr)
		{
			glClearColor(GLclampf(_clr.r), GLclampf(_clr.g), GLclampf(_clr.b), GLclampf(_clr.a));
		}

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::setZCompare(bool _enable)
		{
			if (_enable)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);
		}

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::finishFrame()
		{
			swapBuffers();
		}

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::setShader(const Shader* _shader) {
			const OpenGLShader* shader = static_cast<const OpenGLShader*>(_shader);
			mProgram = shader->program();
			glUseProgram(mProgram);
		}

		//------------------------------------------------------------------------------------------------------------------
		int OpenGLDriver::getUniformLocation(const char* _uniform)
		{
			return glGetUniformLocation(mProgram, _uniform);
		}

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::drawIndexBuffer(int _nIndices, unsigned short * _indices, EPrimitiveType _primitive)
		{
			switch (_primitive)
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
				assert(false); // Error: Trying to render an unimplemented primitive type
			}
		}

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::setAttribBuffer(unsigned _attribId, unsigned _nElements, const float* _buffer)
		{
			_nElements;
			glVertexAttribPointer(_attribId, 1, GL_FLOAT, GL_FALSE, 0, _buffer);
		}

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::setAttribBuffer(unsigned _attribId, unsigned _nElements, const uint16_t* _buffer)
		{
			_nElements;
			glVertexAttribPointer(_attribId, 1, GL_UNSIGNED_SHORT, GL_FALSE, 0, _buffer);
		}

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::setAttribBuffer(unsigned _attribId, unsigned _nElements, const int16_t* _buffer)
		{
			_nElements;
			glVertexAttribPointer(_attribId, 1, GL_SHORT, GL_FALSE, 0, _buffer);
		}

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::setAttribBuffer(unsigned _attribId, unsigned _nElements, const math::Vec2f* _buffer)
		{
			_nElements;
			glVertexAttribPointer(_attribId, 2, GL_FLOAT, GL_FALSE, 0, _buffer);
		}

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::setAttribBuffer(unsigned _attribId, unsigned _nElements, const math::Vec3f* _buffer)
		{
			_nElements;
			glVertexAttribPointer(_attribId, 3, GL_FLOAT, GL_FALSE, 0, _buffer);
		}

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::setUniform(int _uniformId, float _value)
		{
			glUniform1f(_uniformId, _value);
		}

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::setUniform(int _uniformId, const math::Vec2f& _value)
		{
			glUniform2f(_uniformId, _value.x, _value.y);
		}

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::setUniform(int _uniformId, const math::Vec3f& _value)
		{
			glUniform3f(_uniformId, _value.x, _value.y, _value.z);
		}

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::setUniform(int _uniformId, const math::Mat44f& _value)
		{
			glUniformMatrix4fv(_uniformId, 1, true, &_value[0][0]);
		}

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::setUniform(int _uniformId, const Color& _value)
		{
			glUniform4f(_uniformId, _value.r, _value.g, _value.b, _value.a);
		}

	}	// namespace video
}	// namespace rev