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
			void clear() {
				floats.clear();
				vec3s.clear();
				vec4s.clear();
				mat4s.clear();
				mat4vs.clear();
				textures.clear();
			}

			template<class T> using ParamList = std::vector<std::pair<int,T>>;

			ParamList<float> floats;
			ParamList<math::Vec3f> vec3s;
			ParamList<math::Vec4f> vec4s;
			ParamList<math::Mat44f> mat4s;
			ParamList<std::vector<math::Mat44f>> mat4vs;
			ParamList<Texture2d>	textures;

			size_t addParam(int pos, float x) { floats.push_back({pos, x}); return floats.size() -1; }
			size_t addParam(int pos, math::Vec3f x) { vec3s.push_back({pos, x}); return vec3s.size() -1; }
			size_t addParam(int pos, math::Vec4f x) { vec4s.push_back({pos, x}); return vec4s.size() -1; }
			size_t addParam(int pos, math::Mat44f x) { mat4s.push_back({pos, x}); return mat4s.size() -1; }
			size_t addParam(int pos, Texture2d x) { textures.push_back({pos, x}); return textures.size() -1; }
		};

		// Commands
		struct Command
		{
			enum Opcode {
				BeginPass,
				BindFrameBuffer,
				SetViewport,
				SetScissor,
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

		void beginPass(RenderPass& pass)
		{
			assert(pass.isValid());
			m_commands.push_back({ Command::BeginPass, pass.id() });
		}
		void bindFrameBuffer(FrameBuffer fb)
		{
			assert(fb.isValid());
			m_commands.push_back({ Command::BindFrameBuffer, fb.id() });
		}

		void setViewport(const math::Vec2u& start, const math::Vec2u& size)
		{
			m_commands.push_back({ Command::SetViewport, (int32_t)m_rect.size() });
			m_rect.push_back({start, size});
		}

		void setScissor(const math::Vec2u& start, const math::Vec2u& size)
		{
			m_commands.push_back({ Command::SetScissor, (int32_t)m_rect.size() });
			m_rect.push_back({start, size});
		}

		void setPipeline(Pipeline pipeline)
		{
			assert(pipeline.isValid());
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
		void drawTriangles(int numIndices, IndexType indexType, void* offset)
		{
			m_commands.push_back({Command::DrawTriangles, (int32_t)m_draws.size() });
			m_draws.push_back({numIndices, indexType, offset});
		}
		void drawLines(int nVertices, IndexType);

		// Command buffer lifetime
		void clear() {
			m_commands.clear();
			m_uniforms.clear();
			m_draws.clear();
		}

		struct DrawPayload
		{
			int nIndices;
			IndexType indexType;
			void* offset = nullptr;
		};

		struct WindowRect
		{
			math::Vec2u pos, size;
		};

		// Access
		const std::vector<Command> commands() const { return m_commands; }
		const UniformBucket& getUniforms(size_t i) const { return m_uniforms[i]; }
		const DrawPayload& getDraw(size_t i) const { return m_draws[i]; }
		const WindowRect& getRect(size_t i) const { return m_rect[i]; }

	private:

		std::vector<Command> m_commands;
		std::vector<UniformBucket> m_uniforms;
		std::vector<DrawPayload> m_draws;
		std::vector<WindowRect> m_rect;
	};
}
