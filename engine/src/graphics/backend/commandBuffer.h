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
#include "pipeline.h"

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
			//ParamList<RenderQueue::Texture>	textures;
		};

		// Commands
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
			int32_t payload;
		};

		enum class IndexType
		{
			U8,
			U16,
			U32
		};

		struct DrawPayload
		{
			int nIndices;
			IndexType indexType;
		};

		void setPipeline(const Pipeline& pipeline)
		{
			m_commands.push_back({Command::SetPipeline, pipeline.id});
		}
		void setUniformData(const UniformBucket& uniformBucket)
		{
			m_commands.push_back({Command::SetUniform, (int32_t)m_uniforms.size() });
			m_uniforms.push_back(uniformBucket);
		}
		void setVertexData(const unsigned& vao)
		{
			m_commands.push_back({Command::SetVtxData, (int32_t)vao});
		}
		void drawTriangles(int numIndices, IndexType indexType)
		{
			m_commands.push_back({Command::DrawTriangles, (int32_t)m_draws.size() });
			m_draws.push_back({numIndices, indexType});
		}
		void drawLines(int nVertices, IndexType);

		// Command buffer lifetime
		void clear() {
			m_commands.clear();
			m_uniforms.clear();
			m_draws.clear();
		}

		// Access
		const std::vector<Command> commands() const { return m_commands; }
		const UniformBucket& getUniforms(size_t i) const { return m_uniforms[i]; }
		const DrawPayload& getDraw(size_t i) const { return m_draws[i]; }

	private:

		std::vector<Command> m_commands;
		std::vector<UniformBucket> m_uniforms;
		std::vector<DrawPayload> m_draws;
	};
}
