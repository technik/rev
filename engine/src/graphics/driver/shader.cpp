//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include "shader.h"
#include <core/platform/fileSystem/file.h>
#include <core/string_util.h>
#include <core/tools/log.h>
#include <graphics/debug/imgui.h>

#include <vector>

using namespace rev::core;
using namespace std;
/*
namespace rev::gfx {
	
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

	//---------------------------------------------------------------------------------------------------------------------
	std::unique_ptr<Shader>	Shader::createShader(const char* code)
	{
		std::vector<const char*> c;
		c.push_back(code);
		return createShader(c);
	}

	//---------------------------------------------------------------------------------------------------------------------
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
			std::vector<char> ShaderErrorMessage(InfoLogLength+1);
			glGetProgramInfoLog(program, InfoLogLength, NULL, &ShaderErrorMessage[0]);
			ImGui::Begin("Shader Error");
			std::string textMessage = (char*)ShaderErrorMessage.data();
			ImGui::Text("%s", textMessage.c_str());
			glGetShaderiv(program, GL_SHADER_SOURCE_LENGTH, &InfoLogLength);
			ShaderErrorMessage.resize(InfoLogLength+1);
			glGetShaderSource(program, InfoLogLength, NULL, &ShaderErrorMessage[0]);
			std::string completeSource = (char*)ShaderErrorMessage.data();
			ImGui::Text("%s", completeSource.c_str());
			ImGui::End();
 			return nullptr;
		}

		auto shader = std::make_unique<Shader>();
		shader->mGLProgram = program;

		return shader;
	}

	//---------------------------------------------------------------------------------------------------------------------
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
			std::string textMessage = (char*)ShaderErrorMessage.data();
			ImGui::Text("%s", textMessage.c_str());
			glGetShaderiv(_dst, GL_SHADER_SOURCE_LENGTH, &InfoLogLength);
			ShaderErrorMessage.resize(InfoLogLength+1);
			glGetShaderSource(_dst, InfoLogLength, NULL, &ShaderErrorMessage[0]);
			std::string completeSource = (char*)ShaderErrorMessage.data();
			ImGui::Text("%s", completeSource.c_str());
			ImGui::End();
			return false;
		}

		return true;
	}

} // namespace rev::gfx
*/