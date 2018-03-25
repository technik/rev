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
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace rev { namespace graphics {

	class Effect
	{
	public:
		// Effect creation
		Effect(const std::string _code);
		static std::shared_ptr<Effect>	loadFromFile(const std::string& _fileName);

		struct Attribute
		{
			std::string name;
			enum Type
			{
				Integer,
				Scalar,
				Vec2,
				Vec3,
				Vec4,
				Texture2D,
				Texture3D
			};

			// Returns the serialized version of the attribute as defined in shader code
			// when the attribute is present for a given material.
			std::string preprocessorDirective() const;
		};

		const std::vector<Attribute>& attributes() const { return m_attributes; }

	private:
		std::vector<Attribute>	m_attributes;
		// TODO: Support shader permutations by defining #pragma shader_option in a shader
		// when the material enables the option, the shader option will be #defined in the material
		// Advanced uses may allow enumerated or integer values for the options
		// TODO: Let a shader specify that it requires specific data from the vertex or fragment stages
		// This should allow optimization of shaders when some computations won't be used.
	};

}}
