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

#include "../device.h"
#include "renderPassOpenGL.h"
#include "../pipeline.h"
#include <vector>

namespace rev :: gfx
{
	class DeviceOpenGL : public Device
	{
	public:
		RenderQueue& renderQueue() override
		{
			return *m_renderQueue;
		}

		// --- Stuff allocation ---
		// Texture sampler
		TextureSampler	createTextureSampler(const TextureSampler::Descriptor&) override;
		void			destroyTextureSampler(TextureSampler) override;

		// Texture
		Texture2d	createTexture2d(const Texture2d::Descriptor&) override;
		void		destroyTexture2d(Texture2d) override;

		// Frame buffers
		FrameBuffer createFrameBuffer(const FrameBuffer::Descriptor&) override;
		
		// Render passes
		RenderPass* createRenderPass(const RenderPass::Descriptor& desc) override
		{
			return new RenderPassOpenGL(desc);
		}
		void destroyRenderPass(const RenderPass&) override {}

		// Pipeline
		Pipeline::ShaderModule createShaderModule(const Pipeline::ShaderModule::Descriptor&) override;
		Pipeline createPipeline(const Pipeline::Descriptor&) override;

		// OpenGL specifics
		virtual FrameBuffer defaultFrameBuffer() = 0; // Frame buffer of the main window
		void bindPipeline(int32_t pipelineId);

		// Buffers
		Buffer allocateStaticVtxBuffer(size_t byteSize, const void* data) override
		{
			Buffer buffer;
			buffer.id = allocateStaticBuffer(GL_ARRAY_BUFFER, byteSize, data);
			return buffer;
		}

		Buffer allocateIndexBuffer(size_t byteSize, const void* data) override
		{
			Buffer buffer;
			buffer.id = allocateStaticBuffer(GL_ELEMENT_ARRAY_BUFFER, byteSize, data);
			return buffer;
		}

	protected:

		GLuint allocateStaticBuffer(GLenum target, size_t byteSize, const void* data)
		{
			GLuint vbo;
			glGenBuffers(1,&vbo);
			glBindBuffer(target, vbo);
			glBufferData(target, byteSize, data, GL_STATIC_DRAW);
			glBindBuffer(target, 0);
			return vbo;
		}

		struct PipelineInfo
		{
			Pipeline::Descriptor desc;
			GLuint nativeName;
		};

		DeviceOpenGL() = default;

		static GLenum getInternalFormat(const Texture2d::Descriptor&);

		RenderQueue* m_renderQueue = nullptr;
		std::vector<TextureSampler::Descriptor> m_textureSamplers;
		std::vector<PipelineInfo>	m_pipelines;
	};
}
