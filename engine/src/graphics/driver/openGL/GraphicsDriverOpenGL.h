//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include "windows/GraphicsDriverOpenGLWindows.h"
#include "android/GraphicsDriverOpenGLAndroid.h"

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

		void onWindowResize(const math::Vec2u& _size)
		{
			nativeWindow()->size = _size;
			mFrameBuffer->resize(_size);
		}

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