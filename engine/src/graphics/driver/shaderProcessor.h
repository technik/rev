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
#pragma once

#include <string>
#include <vector>

namespace rev::gfx
{
	class ShaderProcessor
	{
	public:
		struct Uniform
		{
			enum Type
			{
				//Integer,
				Scalar,
				Vec2,
				Vec3,
				Vec4,
				Mat2,
				Mat3,
				Mat4,
				Texture2D
				//Texture3D
			};

			Type type;
			std::string name;
			int location = -1;

			std::string preprocessorDirective() const;
		};

		struct MetaData
		{
			std::vector<std::string>	pragmas;
			std::vector<Uniform>		uniforms;
			std::vector<std::string>	dependencies;

			void clear() {
				pragmas.clear();
				uniforms.clear();
				dependencies.clear();
			}
		};

		struct Context
		{
			std::vector<std::string> m_includePathStack;
			std::vector<std::string> m_fileStack;
			std::vector<std::pair<size_t,std::string>> m_pendingCode; // read position, code
		};

		static bool loadCodeFromFile(const std::string& fileName, std::string& out, MetaData& meta);
		
		static bool processCode(Context& context, bool followIncludes, std::string& out, MetaData& meta);

	private:

		static bool processLine(const std::string& line, Context& context, bool followIncludes, MetaData&, std::string& outCode);
		static bool processInclude(const std::string& line, Context& context, bool followIncludes, MetaData&, std::string& outCode);
		static bool processUniform(const std::string& line, MetaData& metadata);
	};

} // namespace rev::gfx