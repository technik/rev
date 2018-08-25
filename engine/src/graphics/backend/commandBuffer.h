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
			ParamList<std::vector<math::Mat44f>> mat4vs;
			ParamList<RenderQueue::Texture>	textures;
		};

		// Commands
		void setPipeline(const RenderQueue::Pipeline&);
		void setUniformData(const UniformBucket&);
		void setVertexData(const RenderQueue::VertexArrayObject&);
		void drawTriangles(int numVertices);
		void drawLines(int nVertices);
		
		struct Command
		{
			enum Opcode {
				SetPipeline,
				SetUniform,
				SetVtxData,
				DrawTriangles,
				DrawLines,
			};

			Opcode command;
			size_t payload;
		};

		// Command buffer lifetime
		void begin();
		void end();
		void clear();

		// Access
		const std::vector<Command> commands() const { m_commands; }
		const std::vector<UniformBucket> uniforms() const { return m_uniforms; }

	private:
		std::vector<Command> m_commands;
		std::vector<UniformBucket> m_uniforms;
	};
}
