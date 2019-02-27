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

namespace rev :: gfx
{
	using Command = CommandBuffer::Command;

	void RenderQueueOpenGL::setUniforms(const CommandBuffer::UniformBucket& bucket)
	{
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
			++texStage;
		}

		for(auto& out : bucket.computeOut)
		{
			// Override images
			if(out.second.isValid())
				glBindImageTexture(out.first, out.second.id(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
			break;
		}
	}

	//----------------------------------------------------------------------------------------------
	RenderQueueOpenGL::RenderQueueOpenGL(DeviceOpenGL& device)
		: m_device(device)
	{}

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
					break;
				}
				case Command::BindFrameBuffer:
				{
					m_device.bindFrameBuffer(cmd.payload);
				}
				case Command::SetPipeline:
				{
					m_device.bindPipeline(cmd.payload);
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
					break;
				}
				case Command::MemoryBarrier:
				{
					glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
					break;
				}
				case Command::SetComputeProgram:
				{
					glUseProgram(cmd.payload);
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
			}
		}
	}
}
