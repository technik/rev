//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include <memory>
#include "openGL/openGL.h"

namespace rev { namespace graphics {

	class Shader {
	public:
		static std::unique_ptr<Shader>	createShader(const char* code);

		void bind() const { glUseProgram(mGLProgram); }

	private:
		static bool createSubprogram(const char* _code, GLenum _shaderType, GLuint& _dst);

		GLuint mGLProgram = 0;
	};

}}	// namespace rev::graphics
