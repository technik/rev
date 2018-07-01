//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include "shader.h"
#include "openGL/openGL.h"
#include <core/platform/fileSystem/file.h>
#include <core/string_util.h>
#include <core/tools/log.h>
#include <graphics/debug/imgui.h>

#include <vector>

using namespace rev::core;
using namespace std;

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
			std::vector<char> ProgramErrorMessage(InfoLogLength+1);
			glGetProgramInfoLog(program, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			Log::error(ProgramErrorMessage.data());
 			return nullptr;
		}

		auto shader = std::make_unique<Shader>();
		shader->mGLProgram = program;

		return shader;
	}

	//---------------------------------------------------------------------------------------------------------------------
	std::string Shader::loadCodeFromFile(const std::string& fileName)
	{
		string fullCode;
		vector<pair<size_t,string>> pendingCode; // read position, code
		vector<string> includePaths;

		// load code from the entry file
		core::File baseFile(fileName);
		pendingCode.emplace_back(0,baseFile.bufferAsText());
		includePaths.push_back(core::getPathFolder(fileName));

		const string includeLabel = "#include";

		// TODO: Parsing code files line by line may be more robust (e.g. do not detect includes inside comments)
		// Parse code
		while(!pendingCode.empty())
		{
			auto& [lineStart, code] = pendingCode.back();
			// Extract line
			auto lineEnd = code.find('\n', lineStart);
			bool isLastLine = (lineEnd == std::string::npos);
			if(isLastLine) // Last line
			{
				lineEnd = code.length();
			}
			// Process line
			if(lineStart < code.length())
			{
				if((lineStart+includeLabel.length()<code.length()) && code.substr(lineStart, includeLabel.length()) == includeLabel) // Include line
				{
					// Find the include path
					auto startPos = code.find('"', lineStart+includeLabel.length()) + 1;
					auto endPos = code.find('"', startPos);

					auto pathAppend = code.substr(startPos, endPos-startPos);
					if(pathAppend.empty())
					{
						cout << "Error parsing shader include. Empty include processing " << fileName << "\n";
						return nullptr;
					}

					auto fullPath = includePaths.back() + pathAppend;
					core::File includedFile(fullPath);
					if(includedFile.sizeInBytes() == 0)
					{
						cout << "Error: unable to find include file " << fullPath << " while parsing shader file " << fileName << "\n";
						return nullptr;
					}
					lineStart = lineEnd+1; // Prepare to keep reading after end of file
					includePaths.push_back(core::getPathFolder(fullPath));
					pendingCode.emplace_back(0, includedFile.bufferAsText());
				}
				else // Regular line
				{
					fullCode.append(code.substr(lineStart, lineEnd-lineStart+1));
				}
			}
			// Prepare next iteration
			lineStart = lineEnd+1;
			if(isLastLine)
			{
				fullCode.append("\n");
				pendingCode.pop_back();
				includePaths.pop_back();
			}
		}

		return fullCode;
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

}}	// namespace rev::graphics

