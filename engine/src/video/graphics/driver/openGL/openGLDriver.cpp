//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/06
//----------------------------------------------------------------------------------------------------------------------
// Interface with OpenGL
#include "openGLDriver.h"

#ifndef ANDROID
#	ifdef _WIN32
#		include <Windows.h>
#	endif
#	include <GL/gl.h>
#	ifdef _WIN32
#		include "glew.h"
#	else
#		include <GL/glew.h>
#	endif // !_WIN32
#endif // !ANDROID

#include <video/basicTypes/color.h>
#include <video/graphics/shader/shader.h>
#include <video/graphics/renderer/types/renderTarget.h>
#include <iostream>

namespace rev {
	namespace video {

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
		void OpenGLDriver::setCulling(ECulling _culling) {
			if (_culling == ECulling::eNone) {
				glDisable(GL_CULL_FACE);
			}
			else {
				glEnable(GL_CULL_FACE);
				glCullFace(_culling==ECulling::eFront?GL_FRONT:GL_BACK);
			}
		}

#ifndef ANDROID
		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::setMultiSampling(bool _enable) {
			if(_enable)
				glEnable(GL_MULTISAMPLE);
			else
				glDisable(GL_MULTISAMPLE);
		}
#endif // !ANDROID

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::finishFrame()
		{
			swapBuffers();
		}

		//----------------------------------------------------------------------------------------------------------------------
		void logGlError() {
			GLenum error = glGetError();
			if (error) {
				switch (error) {
				case GL_INVALID_ENUM:
					std::cout << "GL_INVALID_ENUM\n";
					break;
				case GL_INVALID_VALUE:
					std::cout << "GL_INVALID_VALUE\n";
					break;
				case GL_INVALID_OPERATION:
					std::cout << "GL_INVALID_OPERATION\n";
					break;
				default:
					std::cout << "Other GL error\n";
					break;
				}
			}
		}

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::setShader(const Shader::Ptr _shader) {
			if(!_shader)
				return;
			//logGlError();
			mProgram = _shader->program();
			glUseProgram(mProgram);
			//logGlError();
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			//logGlError();
		}

		//------------------------------------------------------------------------------------------------------------------
		int OpenGLDriver::getUniformLocation(const char* _uniform)
		{
			int loc = glGetUniformLocation(mProgram, _uniform);
			/*if (loc == -1) {
				GLenum error = glGetError();
				std::cout << "Uniform " << _uniform << " not found. Error ";
				switch (error)
				{
				case GL_INVALID_VALUE:
					std::cout << "GL_INVALID_VALUE\n";
					break;
				case GL_INVALID_OPERATION:
					std::cout << "GL_INVALID_OPERATION\n";
					break;
				default:
					std::cout << error << "\n";
					break;
				}
			}*/
			return loc;
		}
		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::setRenderTarget(RenderTarget* _rt) {
			glFinish();
			if (_rt) {
				glBindFramebuffer(GL_FRAMEBUFFER, _rt->glId());
				GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
				if (status != GL_FRAMEBUFFER_COMPLETE)
				{
					std::cout << "Incomplete fb\n";
					assert(false);
				}
				//GLenum db = GL_COLOR_ATTACHMENT0;
				//glDrawBuffers(1, &db);
			}
			else {
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
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
			case EPrimitiveType::points:
				glDrawElements(GL_POINTS, _nIndices, GL_UNSIGNED_SHORT, _indices);
				break;
			default:
				assert(false); // Error: Trying to render an unimplemented primitive type
			}
		}

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::setAttribBuffer(unsigned _attribId, unsigned, const float* _buffer)
		{
			glVertexAttribPointer(_attribId, 1, GL_FLOAT, GL_FALSE, 0, _buffer);
		}

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::setAttribBuffer(unsigned _attribId, unsigned, const uint16_t* _buffer)
		{
			glVertexAttribPointer(_attribId, 1, GL_UNSIGNED_SHORT, GL_FALSE, 0, _buffer);
		}

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::setAttribBuffer(unsigned _attribId, unsigned , const int16_t* _buffer)
		{
			glVertexAttribPointer(_attribId, 1, GL_SHORT, GL_FALSE, 0, _buffer);
		}

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::setAttribBuffer(unsigned _attribId, unsigned, const math::Vec2f* _buffer)
		{
			glVertexAttribPointer(_attribId, 2, GL_FLOAT, GL_FALSE, 0, _buffer);
		}

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::setAttribBuffer(unsigned _attribId, unsigned, const math::Vec3f* _buffer)
		{
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
		void OpenGLDriver::setUniform(int _uniformId, const math::Mat33f& _value)
		{
			glUniformMatrix3fv(_uniformId, 1, true, &_value[0][0]);
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

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::setUniform(int _uniformId, const Texture* _tex) {
			//logGlError();
			if(hasTexStage(_tex)) {
				glActiveTexture(mAssignedTexStages[_tex->glId()]);
				glBindTexture(GL_TEXTURE_2D, _tex->glId());
			} else {
				assignTexStage(_tex);
			}
			//logGlError();
			glUniform1i(_uniformId, mAssignedTexStages[_tex->glId()] - GL_TEXTURE0);
			//logGlError();
		}

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::assignTexStage(const Texture* _tex) {
			glActiveTexture(mCurTexStage);
			glBindTexture(GL_TEXTURE_2D, _tex->glId());
			mAssignedTexStages[_tex->glId()] = mCurTexStage;

			mCurTexStage++;
			if (mCurTexStage > GL_TEXTURE31)
				mCurTexStage = GL_TEXTURE0;
		}

		//------------------------------------------------------------------------------------------------------------------
		bool OpenGLDriver::hasTexStage(const Texture* _tex) const {
			return mAssignedTexStages.find(_tex->glId()) != mAssignedTexStages.end();
		}

	}	// namespace video
}	// namespace rev