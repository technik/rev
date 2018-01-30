//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include "windows/GraphicsDriverOpenGLWindows.h"
#include "android/GraphicsDriverOpenGLAndroid.h"

#include <math/algebra/vector.h>

namespace rev { namespace graphics {

	class GraphicsDriverGL : public GraphicsDriverGLBase
	{
	public:
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
	};

}}