//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2018 Carmelo J Fdez-Aguera
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "shaderProcessor.h"
#include <iostream>
#include <core/platform/fileSystem/file.h>
#include <core/string_util.h>

using namespace std;

namespace rev::gfx
{

	//----------------------------------------------------------------------------------------------
	string ShaderProcessor::Uniform::preprocessorDirective() const
	{
		string typePrefix;
		if(type == Scalar)
			typePrefix = "float";
		else if(type == Vec3)
			typePrefix = "vec3";
		else if(type == Vec4)
			typePrefix = "vec4";
		else if(type == Texture2D)
			typePrefix = "sampler2D";

		return "#define " + typePrefix + '_' + name + "\n";
	}

	//------------------------------------------------------------------------------------------------------------------
	bool ShaderProcessor::loadCodeFromFile(const std::string& fileName, std::string& outCode, MetaData& metadata)
	{
		// load code from the entry file
		core::File baseFile(fileName);
		if(!baseFile.sizeInBytes())
			return false;

		// Init parsing context
		Context context;
		context.m_fileStack.emplace_back(fileName);
		context.m_includePathStack.push_back(core::getPathFolder(fileName));
		context.m_pendingCode.emplace_back(0, baseFile.bufferAsText());

		// Add first dependency
		metadata.dependencies.emplace_back(fileName);

		// Parse code
		return processCode(context, true, outCode, metadata);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool ShaderProcessor::processCode(Context& context, bool followIncludes, std::string& outCode, MetaData& metadata)
	{
		if(context.m_includePathStack.empty())
			context.m_includePathStack.emplace_back("");
		// Parse code
		while(!context.m_pendingCode.empty())
		{
			auto& [lineStart, code] = context.m_pendingCode.back();
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
				auto line = code.substr(lineStart, lineEnd-lineStart+1);
				lineStart = lineEnd+1; // Prepare next line before context stacks get resized and invalidate references
				auto success = processLine(line, context, followIncludes, metadata, outCode);

				if(!success)
					return false;
			}
			// Prepare next iteration
			if(isLastLine)
			{
				outCode.append("\n");
				context.m_pendingCode.pop_back();
				context.m_includePathStack.pop_back();
			}
		}

		return true;
	}

	//---------------------------------------------------------------------------------------------------------------------
	bool ShaderProcessor::processLine(const std::string& line, Context& context, bool followIncludes, MetaData& metadata, std::string& outCode)
	{
		const string includeLabel = "#include";
		const string pragmaLabelA = "#pragma ";
		const string pragmaLabelB = "#pragma	";
		const string uniformLabel = "uniform";

		bool success = true;

		if(line.substr(0, includeLabel.length()) == includeLabel) // Include line
		{
			success = processInclude(line, context, followIncludes, metadata, outCode);
		}
		else if(line.substr(0, pragmaLabelA.length()) == pragmaLabelA || line.substr(0, pragmaLabelB.length()) == pragmaLabelB)
		{
			auto payloadStart = line.find_first_not_of(" \t", pragmaLabelA.length());
			string payload = line.substr(payloadStart, line.length()-payloadStart-1);
			metadata.pragmas.push_back(payload);
		}
		else if(line.find("uniform") != std::string::npos)
		{
			if(line.compare(0, 6, "layout") == 0)
				success = processUniform(line, metadata);
			//else
				outCode.append(line);
		}
		else // Regular line
		{
			outCode.append(line);
		}

		return success;
	}

	//---------------------------------------------------------------------------------------------------------------------
	bool ShaderProcessor::processInclude(const std::string& line, Context& context, bool followIncludes, MetaData& metadata, std::string& outCode)
	{
		const string includeLabel = "#include";

		// Find the include path
		auto startPos = line.find('"', includeLabel.length()) + 1;
		auto endPos = line.find('"', startPos);

		auto pathAppend = line.substr(startPos, endPos-startPos);
		if(pathAppend.empty())
		{
			cout << "Error parsing shader include. Empty include processing " << context.m_fileStack.back() << "\n";
			return false;
		}

		auto fullPath = context.m_includePathStack.back() + pathAppend;
		metadata.dependencies.emplace_back(fullPath);

		if(followIncludes)
		{
			core::File includedFile(fullPath);
			if(includedFile.sizeInBytes() == 0)
			{
				cout << "Error: unable to find include file " << fullPath << " while parsing shader file " << context.m_fileStack.back() << "\n";
				return false;
			}
			context.m_includePathStack.push_back(core::getPathFolder(fullPath));
			context.m_fileStack.emplace_back(fullPath);
			context.m_pendingCode.emplace_back(0, includedFile.bufferAsText());
		}

		return true;
	}

	//---------------------------------------------------------------------------------------------------------------------
	bool ShaderProcessor::processUniform(const std::string& line, MetaData& metadata)
	{
		const string uniformToken = "uniform";
		const string samplerTag = "sampler";
		const string vectorTag = "vec";
		const string matrixTag = "mat";
		Uniform prop;
		// Parse uniform location, and store it in the property

		// Read location
		auto loc_pos = line.find_first_not_of(" \t", line.find('=')+1);
		prop.location = stoi(line.substr(loc_pos));

		// Read type
		auto typeStart = line.find_first_not_of(" \t", line.find(uniformToken)+uniformToken.length());
		auto typeSubstr = line.substr(typeStart);
		// Vectors
		if(typeSubstr.substr(0, vectorTag.length()) == vectorTag)
		{
			if(typeSubstr[3] == '2')
			{
				prop.type = Uniform::Vec2;
			} else if(typeSubstr[3] == '3')
			{
				prop.type = Uniform::Vec3;
			} else
			{
				prop.type = Uniform::Vec4;
			}
		} // Matrices
		else if(typeSubstr.substr(0, matrixTag.length()) == matrixTag)
		{
			if(typeSubstr[3] == '2')
			{
				prop.type = Uniform::Mat2;
			} else if(typeSubstr[3] == '3')
			{
				prop.type = Uniform::Mat3;
			} else
			{
				prop.type = Uniform::Mat4;
			}
		} // Textures
		else if(typeSubstr.substr(0, samplerTag.length()) == samplerTag)
		{
			prop.type = Uniform::Texture2D;
		}
		else // float
		{
			prop.type = Uniform::Scalar;
		}
		auto arg_pos = typeSubstr.find_first_of(" \t");
		auto name_pos = typeSubstr.find_first_not_of(" \t", arg_pos);
		auto name_end = typeSubstr.find_first_of(" \t;", name_pos);
		prop.name = typeSubstr.substr(name_pos, name_end-name_pos);
		metadata.uniforms.push_back(prop);

		return true;
	}

}	// namespace rev::gfx