//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include "shader.h"
#include "openGL/openGL.h"
#include <core/tools/log.h>

#include <vector>

using namespace rev::core;

namespace rev { namespace graphics {

	namespace {	
#ifdef ANDROID
		const std::string GLSL_VERSION = 
			R"(#version 300 es
			precision highp float;
			)";
#else
		const std::string GLSL_VERSION = "#version 430";
#endif
		const std::string COMMON_SHADER_HEADER = 
			GLSL_VERSION + 
			R"(
			)";

		const std::string VTX_SHADER_HEADER = "#define VTX_SHADER\n";
		const std::string PXL_SHADER_HEADER = "#define PXL_SHADER\n";
	}

	std::unique_ptr<Shader>	Shader::createShader(const char* code) {
		GLuint vertexShader = 0, fragmentShader = 0;
		if(!createSubprogram(code, GL_VERTEX_SHADER, vertexShader) ||
			!createSubprogram(code, GL_FRAGMENT_SHADER, fragmentShader))
		{
			return nullptr;
		}

		// Link the program
		auto program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glLinkProgram(program);

		// Check the program
		GLint result = GL_FALSE;
		int InfoLogLength = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &result);
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if ( InfoLogLength > 0 ){
			std::vector<char> ProgramErrorMessage(InfoLogLength+1);
			glGetProgramInfoLog(program, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			Log::error(code);
			Log::error(ProgramErrorMessage.data());
			return nullptr;
		}

		auto shader = std::make_unique<Shader>();
		shader->mGLProgram = program;

		return shader;
	}

	bool Shader::createSubprogram(const char* _code, GLenum _shaderType, GLuint& _dst) {
		const char* code[3] = { COMMON_SHADER_HEADER.data(), nullptr, _code };
		if(_shaderType == GL_VERTEX_SHADER)
			code[1] = VTX_SHADER_HEADER.data();
		else
			code[1] = PXL_SHADER_HEADER.data();
		_dst = glCreateShader(_shaderType);
		glShaderSource(_dst, 3, code, nullptr);
		glCompileShader(_dst);

		GLint result = GL_FALSE;
		int InfoLogLength = 0;
		glGetShaderiv(_dst, GL_COMPILE_STATUS, &result);
		glGetShaderiv(_dst, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if ( InfoLogLength > 0 ){
			std::vector<char> ShaderErrorMessage(InfoLogLength+1);
			glGetShaderInfoLog(_dst, InfoLogLength, NULL, &ShaderErrorMessage[0]);
			Log::error((COMMON_SHADER_HEADER+code[1]+code[2]).c_str());
			Log::error(ShaderErrorMessage.data());
			return false;
		}

		return true;
	}

}}	// namespace rev::graphics

