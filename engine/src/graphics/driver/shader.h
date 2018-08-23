//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include <memory>
#include <graphics/backend/OpenGL/openGL.h>
#include <string>
#include <vector>

namespace rev { namespace graphics {

	class Shader {
	public:
		static std::unique_ptr<Shader>	createShader(const char* code);
		static std::unique_ptr<Shader>	createShader(const std::vector<const char*>& code);

		void bind() const { glUseProgram(mGLProgram); }

	private:
		static bool createSubprogram(const std::vector<const char*> _code, GLenum _shaderType, GLuint& _dst);

		GLuint mGLProgram = 0;
	};

}}	// namespace rev::graphics
