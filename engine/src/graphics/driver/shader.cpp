//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include "shader.h"
#include "openGL/openGL.h"
#include <core/tools/log.h>
#include <graphics/debug/imgui.h>

#include <vector>

using namespace rev::core;

namespace rev { namespace graphics {

	namespace {	
#ifdef ANDROID
		const std::string GLSL_VERSION = 
			R"(#version 300 es
			precision highp float;
			#define ANDROID
			)";
#else
		const std::string GLSL_VERSION = 
			R"(#version 430
			#define _WIN32
			)";
#endif
		const std::string COMMON_SHADER_HEADER = 
			GLSL_VERSION + 
			R"(
			)";

		const std::string VTX_SHADER_HEADER = "#define VTX_SHADER\n";
		const std::string PXL_SHADER_HEADER = "#define PXL_SHADER\n";
	}

	std::unique_ptr<Shader>	Shader::createShader(const char* code)
	{
		std::vector<const char*> c;
		c.push_back(code);
		return createShader(c);
	}

	std::unique_ptr<Shader>	Shader::createShader(const std::vector<const char*>& code)
	{
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
		if (GL_FALSE == result){
			std::vector<char> ProgramErrorMessage(InfoLogLength+1);
			glGetProgramInfoLog(program, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			Log::error(ProgramErrorMessage.data());
 			return nullptr;
		}

		auto shader = std::make_unique<Shader>();
		shader->mGLProgram = program;

		return shader;
	}

	bool Shader::createSubprogram(const std::vector<const char*> _code, GLenum _shaderType, GLuint& _dst)
	{
		std::vector<const char*> code;
		code.push_back(COMMON_SHADER_HEADER.data());
		//const char* code[3] = { COMMON_SHADER_HEADER.data(), nullptr, _code };
		if(_shaderType == GL_VERTEX_SHADER)
			code.push_back(VTX_SHADER_HEADER.data());
		else
			code.push_back(PXL_SHADER_HEADER.data());
		for(auto c : _code)
		{
			if(!c)
			{
				/*ImGui::Begin("Shader Error");
				ImGui::Text("Error building shader: One of the code fragments is null");
				ImGui::End();*/
				return false;
			}
			code.push_back(c);
		}
		_dst = glCreateShader(_shaderType);
		glShaderSource(_dst, code.size(), code.data(), nullptr);
		glCompileShader(_dst);

		GLint result = GL_FALSE;
		int InfoLogLength = 0;
		glGetShaderiv(_dst, GL_COMPILE_STATUS, &result);
		glGetShaderiv(_dst, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if ( GL_FALSE == result ){
			std::vector<char> ShaderErrorMessage(InfoLogLength+1);
			glGetShaderInfoLog(_dst, InfoLogLength, NULL, &ShaderErrorMessage[0]);
			ImGui::Begin("Shader Error");
			ImGui::Text(ShaderErrorMessage.data());
			ImGui::End();
			return false;
		}

		return true;
	}

}}	// namespace rev::graphics

