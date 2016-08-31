//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/06
//----------------------------------------------------------------------------------------------------------------------
// Interface with OpenGL
#include "openGLDriver.h"
#ifdef _WIN32
#include <Windows.h>
#endif
#include <GL/gl.h>
#ifdef _WIN32
#include "glew.h"
#else
#include <GL/glew.h>
#endif // !_WIN32

#include <core/platform/fileSystem/fileSystem.h>
#include <video/basicTypes/color.h>
#include <video/graphics/shader/openGL/openGLShader.h>
#include <video/window/window.h>
#include <iostream>

using namespace rev::core;

namespace rev {
	namespace video {
		//--------------------------------------------------------------------------------------------------------------
		OpenGLDriver::OpenGLDriver(Window* _window) : OpenGLDriverBase(_window), mWindow(_window) {
			GLenum res = glewInit();
			assert(res == GLEW_OK);
			Shader::manager()->setCreator(
				[](const string& _name) -> Shader* {
				string pxlName = _name + ".pxl";
				string vtxName = _name + ".vtx";
					OpenGLShader* shader = OpenGLShader::loadFromFiles(vtxName, pxlName);
					if(shader) {
						auto refreshShader = [=](const string&) {
							// Recreate shader
							shader->~OpenGLShader();
							OpenGLShader::loadFromFiles(vtxName, pxlName, *shader);
						};
						FileSystem::get()->onFileChanged(pxlName) += refreshShader;
						FileSystem::get()->onFileChanged(vtxName) += refreshShader;
					}
					return shader;
				});
			Shader::manager()->setOnRelease([](const string& _name, Shader*) {
				string pxlName = _name + ".pxl";
				string vtxName = _name + ".vtx";
				FileSystem::get()->onFileChanged(pxlName).clear();
				FileSystem::get()->onFileChanged(vtxName).clear();
			});
			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glLineWidth(2.f);
			glPointSize(4.f);

			_window->onResize() += [this]() { 
				setViewport(math::Vec2i(0,0), mWindow->size());
			};
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
		void OpenGLDriver::setCulling(ECulling _culling) {
			if (_culling == ECulling::eNone) {
				glDisable(GL_CULL_FACE);
			}
			else {
				glEnable(GL_CULL_FACE);
				glCullFace(_culling==ECulling::eFront?GL_FRONT:GL_BACK);
			}
		}

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::setMultiSampling(bool _enable) {
			if(_enable)
				glEnable(GL_MULTISAMPLE);
			else
				glDisable(GL_MULTISAMPLE);
		}

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
		void OpenGLDriver::setShader(const Shader* _shader) {
			if(!_shader)
				return;
			const OpenGLShader* shader = static_cast<const OpenGLShader*>(_shader);
			//logGlError();
			mProgram = shader->program();
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
		Texture* OpenGLDriver::createTexture(const math::Vec2u& _size, Texture::EImageFormat _if, Texture::EByteFormat _bf, void* _data, bool _multiSample) {
			TextureGL* tex = new TextureGL;
			tex->data = (uint8_t*)_data;
			GLenum target = _multiSample? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
			glGenTextures(1, &tex->id);
			glBindTexture(target, tex->id);
			GLint format = enumToGl(_if);
			tex->imgFormat = _if;
			GLint byteFormat = enumToGl(_bf);
			tex->byteFormat = _bf;
			tex->size = _size;
			if(_multiSample)
				glTexImage2DMultisample(target, 4, format, _size.x, _size.y, true);
			else {
				glTexImage2D(target, 0, format, _size.x, _size.y, 0, format, enumToGl(_bf), _data);
				glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP);
			}
			return tex;
		}

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::destroyRenderTarget(RenderTarget* _rt) {
			RenderTargetGL* rt = static_cast<RenderTargetGL*>(_rt);
			if(rt->color) {
				TextureGL* tex = static_cast<TextureGL*>(rt->color);
				glDeleteTextures(1, &tex->id);
			}
			if (rt->depth) {
				TextureGL* tex = static_cast<TextureGL*>(rt->color);
				glDeleteTextures(1, &tex->id);
			}
			glDeleteFramebuffers(1, &rt->id);
		}

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::setRenderTarget(RenderTarget* _rt) {
			glFinish();
			if (_rt) {
				RenderTargetGL* rt = static_cast<RenderTargetGL*>(_rt);
				glBindFramebuffer(GL_FRAMEBUFFER, rt->id);
				GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
				if (status != GL_FRAMEBUFFER_COMPLETE)
				{
					std::cout << "Incomplete fb\n";
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
			const TextureGL* tex = static_cast<const TextureGL*>(_tex);
			if(hasTexStage(tex)) {
				glActiveTexture(mAssignedTexStages[tex->id]);
				glBindTexture(GL_TEXTURE_2D, tex->id);
			} else {
				assignTexStage(tex);
			}
			//logGlError();
			glUniform1i(_uniformId, mAssignedTexStages[tex->id] - GL_TEXTURE0);
			//logGlError();
		}

		//------------------------------------------------------------------------------------------------------------------
		void OpenGLDriver::assignTexStage(const TextureGL* _tex) {
			glActiveTexture(mCurTexStage);
			glBindTexture(GL_TEXTURE_2D, _tex->id);
			mAssignedTexStages[_tex->id] = mCurTexStage;

			mCurTexStage++;
			if (mCurTexStage > GL_TEXTURE31)
				mCurTexStage = GL_TEXTURE0;
		}

		//------------------------------------------------------------------------------------------------------------------
		bool OpenGLDriver::hasTexStage(const TextureGL* _tex) const {
			return mAssignedTexStages.find(_tex->id) != mAssignedTexStages.end();
		}

	}	// namespace video
}	// namespace rev