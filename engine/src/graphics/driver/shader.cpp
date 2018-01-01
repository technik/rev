//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include "shader.h"
#include "openGL/openGL.h"

namespace rev { namespace graphics {

	std::unique_ptr<Shader>	Shader::createShader(const char* code) {
		GLuint vertexShader = 0, fragmentShader = 0;
		if(!createSubprogram(code, GL_VERTEX_SHADER, vertexShader) ||
			!createSubprogram(code, GL_VERTEX_SHADER, vertexShader))
		{
			return nullptr;
		}

		auto program = glCreateProgram();

		return nullptr;
	}

}}	// namespace rev::graphics

