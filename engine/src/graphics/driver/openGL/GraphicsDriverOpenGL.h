//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include "windows/GraphicsDriverOpenGLWindows.h"
#include "android/GraphicsDriverOpenGLAndroid.h"

#include <core/tools/log.h>
#include <math/algebra/vector.h>
#include <graphics/driver/DefaultFrameBuffer.h>
#include <memory>

namespace rev { namespace graphics {

	class GraphicsDriverGL : public GraphicsDriverGLBase
	{
	public:
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

	private:
		std::unique_ptr<DefaultFrameBuffer> mFrameBuffer;
	};

}}