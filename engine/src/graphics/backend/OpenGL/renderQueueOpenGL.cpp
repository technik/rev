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
#include "renderQueueOpenGL.h"
#include "../commandBuffer.h"
#include "deviceOpenGL.h"
#include "graphics/debug/imgui.h"

namespace rev :: gfx
{
	using Command = CommandBuffer::Command;

	void RenderQueueOpenGL::setUniforms(const CommandBuffer::UniformBucket& bucket)
	{
		// Update performance counters
		m_numUniforms += bucket.size();
		m_numBackendCalls+= bucket.size();
		m_numTextures += bucket.textures.size();

		// Bind uniforms per type
		for(auto& entry : bucket.floats)
			glUniform1f(entry.first, entry.second);
		for(auto& entry : bucket.vec3s)
			glUniform3f(entry.first, entry.second[0], entry.second[1], entry.second[2]);
		for(auto& entry : bucket.vec4s)
			glUniform4f(entry.first, entry.second[0], entry.second[1], entry.second[2], entry.second[3]);
		for(auto& entry : bucket.mat4s)
			glUniformMatrix4fv(entry.first, 1, !math::Mat44f::is_col_major, entry.second.data());
		for(auto& entry : bucket.mat4vs)
			glUniformMatrix4fv(entry.first, entry.second.size(), !math::Mat44f::is_col_major, entry.second[0].data());
		for(auto& entry : bucket.storageBuffers)
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, entry.first, entry.second.id());
			
		for(auto& tex : bucket.textures)
		{
			auto slotIter = m_textureSlots.find(tex.first);
			if(slotIter == m_textureSlots.end())
			{
				slotIter = m_textureSlots.emplace(tex.first,(int)m_textureSlots.size()).first;
			}
			int texStage = slotIter->second;
			glActiveTexture(GL_TEXTURE0 + texStage);
			glBindTexture(GL_TEXTURE_2D, tex.second.id());
			glUniform1i(tex.first, texStage);
			m_numBackendCalls+=2; // Two more backend calls than other uniforms
			++texStage;
		}

		for(auto& out : bucket.computeOut)
		{
			// Override images
			if(out.second.isValid())
				glBindImageTexture(out.first, out.second.id(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		}
	}

	//----------------------------------------------------------------------------------------------
	RenderQueueOpenGL::RenderQueueOpenGL(DeviceOpenGL& device)
		: m_device(device)
	{}

	//----------------------------------------------------------------------------------------------
	void RenderQueueOpenGL::present()
	{
		resetPerformanceCounters();
	}

	//----------------------------------------------------------------------------------------------
	void RenderQueueOpenGL::submitCommandBuffer(const CommandBuffer& cmdBuffer)
	{
		for(auto& cmd : cmdBuffer.commands())
		{
			switch(cmd.command)
			{
				case Command::BeginPass:
				{
					m_device.bindPass(cmd.payload, *this);
					m_numBackendCalls++;
					break;
				}
				case Command::BindFrameBuffer:
				{
					m_device.bindFrameBuffer(cmd.payload);
					m_numBackendCalls++;
					break;
				}
				case Command::ClearDepth:
				{
					glClearDepth(cmdBuffer.getFloat(cmd.payload));
					m_numBackendCalls++;
					break;
				}
				case Command::ClearColor:
				{
					auto color = cmdBuffer.getColor(cmd.payload);
					glClearColor(color.x(), color.y(), color.z(), color.w());
					m_numBackendCalls++;
					break;
				}
				case Command::Clear:
				{
					auto flags = cmd.payload;
					if (!flags)
						continue;
					auto clearDepth = (flags & (int32_t)Clear::Depth) ? GL_DEPTH_BUFFER_BIT : 0;
					auto clearColor = (flags & (int32_t)Clear::Color) ? GL_COLOR_BUFFER_BIT : 0;
					if (clearDepth)
					{
						glDepthMask(GL_TRUE);
						m_numBackendCalls++;
					}
					glClear(clearDepth | clearColor);
					m_numBackendCalls++;
					break;
				}
				case Command::SetViewport:
				{
					auto rect = cmdBuffer.getRect(cmd.payload);
					glViewport((GLint)rect.pos.x(), (GLint)rect.pos.y(), (GLsizei)rect.size.x(), (GLsizei)rect.size.y());
					m_numBackendCalls++;
					break;
				}
				case Command::SetScissor:
				{
					auto rect = cmdBuffer.getRect(cmd.payload);
					glScissor((GLint)rect.pos.x(), (GLint)rect.pos.y(), (GLsizei)rect.size.x(), (GLsizei)rect.size.y());
					m_numBackendCalls++;
					break;
				}
				case Command::SetPipeline:
				{
					m_device.bindPipeline(cmd.payload);
					m_numBackendCalls++;
					m_numPipelineChanges++;
					break;
				}
				case Command::SetUniform:
				{
					setUniforms(cmdBuffer.getUniforms(cmd.payload));
					break;
				}
				case Command::SetVtxData:
				{
					auto vao = cmd.payload;
					glBindVertexArray(vao);
					m_numBackendCalls++;
					break;
				}
				case Command::DrawTriangles:
				{
					auto& drawInfo = cmdBuffer.getDraw(cmd.payload);
					GLenum indexType = GL_UNSIGNED_SHORT;
					if(drawInfo.indexType == CommandBuffer::IndexType::U8)
						indexType = GL_UNSIGNED_BYTE;
					if(drawInfo.indexType == CommandBuffer::IndexType::U32)
						indexType = GL_UNSIGNED_INT;

					glDrawElements(GL_TRIANGLES, drawInfo.nIndices, indexType, drawInfo.offset);
					m_numTriangles += drawInfo.nIndices / 3;
					m_numBackendCalls++;
					m_numDraws++;
					break;
				}
				case Command::DrawBatches:
				{
					auto& batchInfo = cmdBuffer.getBatch(cmd.payload);
					GLenum indexType = GL_UNSIGNED_SHORT;
					if (batchInfo.indexType == CommandBuffer::IndexType::U8)
						indexType = GL_UNSIGNED_BYTE;
					if (batchInfo.indexType == CommandBuffer::IndexType::U32)
						indexType = GL_UNSIGNED_INT;
					glBindBuffer(GL_DRAW_INDIRECT_BUFFER, batchInfo.batchBuffer.id());
					glMultiDrawElementsIndirect(GL_TRIANGLES, indexType, 0, batchInfo.numBatches, 0);
					m_numBackendCalls+=2;
					m_numDraws++;
					break;
				}
				case Command::DrawLines:
				{
					auto& drawInfo = cmdBuffer.getDraw(cmd.payload);
					GLenum indexType = GL_UNSIGNED_SHORT;
					if(drawInfo.indexType == CommandBuffer::IndexType::U8)
						indexType = GL_UNSIGNED_BYTE;
					if(drawInfo.indexType == CommandBuffer::IndexType::U32)
						indexType = GL_UNSIGNED_INT;

					glDrawElements(GL_LINES, drawInfo.nIndices, indexType, drawInfo.offset);
					m_numBackendCalls++;
					m_numDraws++;
					break;
				}
				case Command::MemoryBarrier:
				{
					glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
					m_numBackendCalls++;
					break;
				}
				case Command::SetComputeProgram:
				{
					glUseProgram(cmd.payload);
					m_numBackendCalls++;
					break;
				}
				case Command::DispatchCompute:
				{
					auto& payload = cmdBuffer.getCompute(cmd.payload);
					// Override images
					if(payload.targetTexture.isValid())
						glBindImageTexture(0, payload.targetTexture.id(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
					glDispatchCompute(payload.groupSize.x(), payload.groupSize.y(), payload.groupSize.z());
					break;
				}
				default:
				{
					assert(false && "Command not implemented");
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void RenderQueueOpenGL::drawPerformanceCounters() const
	{
		ImGui::Text("Triangles: %d", m_numTriangles);
		ImGui::Text("Uniforms: %d", m_numUniforms);
		ImGui::Text("Textures: %d", m_numTextures);
		ImGui::Text("GL calls: %d", m_numBackendCalls);
		ImGui::Text("Draws: %d", m_numDraws);
		ImGui::Text("Pipelines: %d", m_numPipelineChanges);
	}

	//------------------------------------------------------------------------------------------------------------------
	void RenderQueueOpenGL::resetPerformanceCounters()
	{
		m_numTriangles = 0;
		m_numUniforms = 0;
		m_numTextures = 0;
		m_numBackendCalls = 0;
		m_numDraws = 0;
		m_numPipelineChanges = 0;
	}
}
