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

#include <math/algebra/matrix.h>
#include <math/algebra/vector.h>

#include <vector>

#include "renderQueue.h"

namespace rev :: gfx
{
	class CommandBuffer
	{
	public:
		// Command buffer lifetime
		virtual void begin() = 0;
		virtual void end() = 0;

		// Uniforms
		struct UniformBucket
		{
			// Management
			void clear();

			template<class T> using ParamList = std::vector<std::pair<int,T>>;

			ParamList<float> floats;
			ParamList<math::Vec3f> vec3s;
			ParamList<math::Vec4f> vec4s;
			ParamList<math::Mat44f> mat4s;
			ParamList<RenderQueue::Texture>	textures;
		};

		// Commands
		virtual void setPipeline(const RenderQueue::Pipeline&) = 0;
		virtual void setUniformData(const UniformBucket&) = 0;
		virtual void setVertexData(const RenderQueue::VertexArrayObject&) = 0;
		virtual void drawTriangles(int numVertices) = 0;
		virtual void drawLines(int nVertices) = 0;

	};
}
