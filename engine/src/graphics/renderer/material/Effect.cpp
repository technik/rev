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
#include "Effect.h"
#include <sstream>

using namespace std;

namespace rev { namespace graphics {

	//----------------------------------------------------------------------------------------------
	Effect::Effect(const string& _code)
		: m_code(_code)
	{
		istringstream codeStream(_code);
		for(string line; getline(codeStream, line); )
		{
			// Line contains an uniform?
			if(line.compare(0, 6, "layout") == 0)
			{
				Property prop;
				auto arg_pos = line.find("sampler2D");
				if(arg_pos != string::npos)
					prop.type = Property::Texture2D;
				else {
					arg_pos = line.find("vec3");
				}
				if(arg_pos != string::npos)
					prop.type = Property::Vec3;
				arg_pos = line.find_first_of(" \t", arg_pos);
				auto name_pos = line.find_first_not_of(" \t", arg_pos);
				auto name_end = line.find_first_of(" \t;", name_pos);
				prop.name = line.substr(name_pos, name_end-name_pos);
				m_properties.push_back(prop);
			}
		}
	}

	//----------------------------------------------------------------------------------------------
}}
