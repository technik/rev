//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include "windows/GraphicsDriverOpenGLWindows.h"
#include "android/GraphicsDriverOpenGLAndroid.h"

#include <core/tools/log.h>
#include <math/algebra/vector.h>
#include <math/algebra/matrix.h>
#include <graphics/driver/DefaultFrameBuffer.h>
#include <memory>

namespace rev { namespace graphics {

	class GraphicsDriverGL : public GraphicsDriverGLBase
	{
		static GraphicsDriverGL* s_instance;
	public:

		static void initSingleton(GraphicsDriverGL* driver) { s_instance = driver; }
		static GraphicsDriverGL* get() { return s_instance; }

		GraphicsDriverGL(std::unique_ptr<DefaultFrameBuffer> _defaultFrameBuffer)
			: mFrameBuffer(std::move(_defaultFrameBuffer))
		{}

		DefaultFrameBuffer* frameBuffer() const { return mFrameBuffer.get(); }

		// Bind uniforms
		void bindUniform(GLint pos, float f)
		{
			glUniform1f(pos, f);
		}

		void bindUniform(GLint pos, const math::Vec3f& f)
		{
			glUniform3f(pos, f.x(), f.y(), f.z());
		}

		void bindUniform(GLint pos, const math::Vec4f& f)
		{
			glUniform4f(pos, f.x(), f.y(), f.z(), f.w());
		}

		void bindUniform(GLint pos, const math::Mat44f& m)
		{
			glUniformMatrix4fv(pos, 1, !math::Mat44f::is_col_major, m.data());
		}

		void bindUniform(GLint pos, const std::vector<math::Mat44f>& matArray)
		{
			glUniformMatrix4fv(pos, matArray.size(), !math::Mat44f::is_col_major, matArray[0].data());
		}

		// True when errors are found
		static bool checkGLErrors()
		{
			auto error = glGetError();
			if(error != GL_NO_ERROR)
			{
				switch(error) {
					case GL_INVALID_ENUM:
						core::Log::debug("GL error: ", "GL_INVALID_ENUM");
						return true;
					case GL_INVALID_INDEX:
						core::Log::debug("GL error: ", "GL_INVALID_INDEX");
						return true;
					case GL_INVALID_OPERATION:
						core::Log::debug("GL error: ", "GL_INVALID_OPERATION");
						return true;
					case GL_INVALID_VALUE:
						core::Log::debug("GL error: ", "GL_INVALID_VALUE");
						return true;
					case GL_INVALID_FRAMEBUFFER_OPERATION:
						core::Log::debug("GL error: ", "GL_INVALID_FRAMEBUFFER_OPERATION");
						return true;
					default:
						core::Log::debug("GL error: ", "Unknown enum ", error);
						return true;
				}
				// glGetDebugMessageLog
			}
			return false;
		}

		// TODO: Unify interface for texture binding with the other uniforms.
		// Also encapsulate the method used for texture binding (big array vs single textures)
		void bindUniformTexture(GLint pos, GLuint tex)
		{
			glBindTexture(pos, tex);
		}

		bool sRGBFrameBuffer() const { return mSupportSRGBFrameBuffer; }

		GLuint allocateStaticBuffer(GLenum target, size_t byteSize, const void* data)
		{
			GLuint vbo;
			glGenBuffers(1,&vbo);
			glBindBuffer(target, vbo);
			glBufferData(target, byteSize, data, GL_STATIC_DRAW);
			glBindBuffer(target, 0);
			return vbo;
		}

		GLuint deallocateBuffer(GLuint vbo)
		{
			glDeleteBuffers(1, &vbo);
		}

	private:
		std::unique_ptr<DefaultFrameBuffer> mFrameBuffer;
	};

}}