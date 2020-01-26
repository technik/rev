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
#include "device.h"
#include "../shaders/computeShader.h"
#include <graphics/debug/imgui.h>

namespace rev :: gfx
{
	class CommandBuffer
	{
	public:

		struct Metrics
		{
			size_t numCommands = 0;
			size_t numTriangles = 0;
			size_t numUniforms = 0;
			size_t numUniformBuckets = 0;
			size_t numVAO = 0;
			size_t numDraws = 0;
			size_t numDispatchs = 0;
			size_t numPipelineChanges = 0;

			void clear()
			{
				*this = Metrics();
			}

			void draw() const
			{
				ImGui::Text("Commands: %d", numCommands);
				ImGui::Text("Triangles: %d", numTriangles);
				ImGui::Text("Uniforms: %d", numUniforms);
				ImGui::Text("Uniform Buckets: %d", numUniformBuckets);
				ImGui::Text("VAOs: %d", numVAO);
				ImGui::Text("Draws: %d", numDraws);
				ImGui::Text("Dispatchs: %d", numDispatchs);
				ImGui::Text("Pipelines: %d", numPipelineChanges);
			}

			Metrics operator-(const Metrics& other) const
			{
				Metrics res = *this;
				res.numCommands -= other.numCommands;
				res.numTriangles -=other.numTriangles;
				res.numUniforms -=other.numUniforms;
				res.numUniformBuckets -=other.numUniformBuckets;
				res.numVAO -=other.numVAO;
				res.numDraws -=other.numDraws;
				res.numDispatchs -=other.numDispatchs;
				res.numPipelineChanges -=other.numPipelineChanges;
				return res;
			}
		};

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
				computeOut.clear();
			}

			size_t size() const {
				return floats.size()
					+ vec3s.size()
					+ vec4s.size()
					+ mat4s.size()
					+ mat4vs.size()
					+ textures.size()
					+ computeOut.size();
			}

			template<class T> using ParamList = std::vector<std::pair<int,T>>;

			ParamList<float> floats;
			ParamList<math::Vec3f> vec3s;
			ParamList<math::Vec4f> vec4s;
			ParamList<math::Mat44f> mat4s;
			ParamList<std::vector<math::Mat44f>> mat4vs;
			ParamList<Texture2d>	textures;
			ParamList<Texture2d>	computeOut;
			ParamList<Buffer>	storageBuffers;

			size_t addParam(int pos, float x) { floats.push_back({pos, x}); return floats.size() -1; }
			size_t addParam(int pos, math::Vec3f x) { vec3s.push_back({pos, x}); return vec3s.size() -1; }
			size_t addParam(int pos, math::Vec4f x) { vec4s.push_back({pos, x}); return vec4s.size() -1; }
			size_t addParam(int pos, math::Mat44f x) { mat4s.push_back({pos, x}); return mat4s.size() -1; }
			size_t addParam(int pos, Texture2d x) { assert(x.isValid()); textures.push_back({pos, x}); return textures.size() -1; }
			size_t addParam(int pos, Buffer x) { assert(x.isValid()); storageBuffers.push_back({pos, x}); return storageBuffers.size() -1; }
			size_t addComputeOutput(int pos, Texture2d x) { computeOut.push_back({pos, x}); return computeOut.size() -1; }
		};

		// Commands
		struct Command
		{
			enum Opcode {
				BeginPass,
				BindFrameBuffer,
				ClearDepth,
				ClearColor,
				Clear,
				SetViewport,
				SetScissor,
				SetPipeline,
				SetUniform,
				SetVtxData,
				DrawTriangles,
				DrawLines,
				MemoryBarrier,
				DispatchCompute,
				SetComputeProgram,
				StreamWrite,
			};

			Opcode command;
			int32_t payload;
		};

		enum class MemoryBarrier : int32_t
		{
			ImageAccess = 0
		};

		enum class IndexType
		{
			U8,
			U16,
			U32
		};

		// Deprecated?
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

		void clearDepth(float d)
		{
			m_commands.push_back({ Command::ClearDepth, (int32_t)m_clearDepths.size() });
			m_clearDepths.push_back(d);
		}

		void clearColor(const math::Vec4f& color)
		{
			m_commands.push_back({ Command::ClearColor, (int32_t)m_clearColors.size() });
			m_clearColors.push_back(color);
		}

		void clear(Clear flags)
		{
			m_commands.push_back({ Command::Clear, (int32_t)flags });
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
			m_metrics.numPipelineChanges++;
		}

		void setUniformData(const UniformBucket& uniformBucket)
		{
			m_commands.push_back({Command::SetUniform, (int32_t)m_uniforms.size() });
			m_uniforms.push_back(uniformBucket);
			m_metrics.numUniforms += uniformBucket.size();
		}

		void setVertexData(const unsigned& vao)
		{
			m_commands.push_back({Command::SetVtxData, (int32_t)vao});
			m_metrics.numVAO++;
		}

		void drawTriangles(int numIndices, IndexType indexType, void* offset)
		{
			m_commands.push_back({Command::DrawTriangles, (int32_t)m_draws.size() });
			m_draws.push_back({numIndices, indexType, offset});
			m_metrics.numTriangles += numIndices / 3;
		}

		void drawLines(int nVertices, IndexType indexType)
		{
			assert(false && "Not implemented, will always draw 0 lines");
			m_commands.push_back({ Command::DrawLines, (int32_t)m_draws.size() });
			m_draws.push_back({ 0, indexType, 0 });
		}

		void memoryBarrier(MemoryBarrier barrier)
		{
			m_commands.push_back({Command::MemoryBarrier, (int32_t)barrier});
		}

		void setComputeProgram(ComputeShader program)
		{
			m_commands.push_back({Command::SetComputeProgram, (int32_t)program.id() });
		}

		// Compute
		void dispatchCompute(Texture2d targetTexture, const math::Vec3i& groupSize)
		{
			m_commands.push_back({Command::DispatchCompute, (int32_t)m_computes.size() });
			m_computes.push_back({targetTexture, groupSize});
		}

		// Command buffer lifetime
		void clear() {
			m_metrics.clear();
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

		struct ComputePayload
		{
			Texture2d targetTexture;
			math::Vec3i groupSize;
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
		const ComputePayload& getCompute(size_t i) const { return m_computes[i]; }
		float getFloat(size_t i) const { return m_clearDepths[i]; }
		auto& getColor(size_t i) const { return m_clearColors[i]; }

		Metrics metrics() const
		{
			Metrics completeMetrics = m_metrics;
			completeMetrics.numCommands = m_commands.size();
			completeMetrics.numDraws = m_draws.size();
			completeMetrics.numDispatchs = m_computes.size();
			completeMetrics.numUniformBuckets = m_uniforms.size();
			return completeMetrics;
		}

	private:
		Metrics m_metrics;

		std::vector<math::Vec4f> m_clearColors;
		std::vector<float> m_clearDepths;
		std::vector<Command> m_commands;
		std::vector<UniformBucket> m_uniforms;
		std::vector<DrawPayload> m_draws;
		std::vector<WindowRect> m_rect;
		std::vector<ComputePayload> m_computes;
	};
}
