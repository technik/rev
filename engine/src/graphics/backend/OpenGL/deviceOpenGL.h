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
		virtual void destroyFrameBuffer(FrameBuffer) override;
		void bindFrameBuffer(FrameBuffer) override;
		
		// Render passes
		void bindPass(int32_t pass, RenderQueue& queue) override;
		RenderPass* createRenderPass(const RenderPass::Descriptor& desc) override;
		void destroyRenderPass(const RenderPass&) override {}

		// Pipeline
		Pipeline::ShaderModule createShaderModule(const Pipeline::ShaderModule::Descriptor&) override;
		Pipeline createPipeline(const Pipeline::Descriptor&) override;

		// Compute shaders
		ComputeShader createComputeShader(const std::vector<std::string>& code) override;
		void destroyComputeShader(const ComputeShader& shader) override;

		// OpenGL specifics
		virtual FrameBuffer defaultFrameBuffer() = 0; // Frame buffer of the main window
		void bindPipeline(int32_t pipelineId);

		// Buffers
		Buffer allocateBuffer(size_t byteSize, BufferUpdateFrequency, BufferUsageTarget, const void* data = nullptr) override;
		void deallocateBuffer(Buffer) override;
		void resubmitBufferData(Buffer handle, size_t byteSize, BufferUpdateFrequency freq, BufferUsageTarget target, const void* data) override;
		void* mapBuffer(Buffer buffer, BufferUsageTarget usage, size_t offset, size_t length) override;
		void unmapBuffer(Buffer buffer, BufferUsageTarget usage) override;

	protected:

		struct PipelineInfo
		{
			Pipeline::Descriptor desc;
			GLuint nativeName;
		};

		struct FrameBufferInfo
		{
			GLuint frameBufferGLname;
			GLenum colorAttachments[cMaxFBAttachments];
			int numColorAttachs;
			bool sRGB = false;
			bool isValid = true;
		};

		DeviceOpenGL() = default;

		static GLenum getImageFormat(const Texture2d::Descriptor& descriptor);
		static GLenum getInternalFormat(const Texture2d::Descriptor&);
		static GLenum toGL(BufferUpdateFrequency);
		static GLenum toGL(BufferUsageTarget);

		RenderQueue* m_renderQueue = nullptr;
		std::vector<TextureSampler::Descriptor> m_textureSamplers;
		std::vector<PipelineInfo>	m_pipelines;
		std::vector<RenderPassOpenGL*>	m_passes;
		std::vector<FrameBufferInfo> m_frameBuffers;

		void readDeviceLimits();
	};
}
