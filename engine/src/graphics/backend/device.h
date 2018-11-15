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
#include "frameBuffer.h"
#include "renderQueue.h"
#include "texture2d.h"
#include "textureSampler.h"
#include "pipeline.h"

namespace rev :: gfx
{
	class RenderPass;

	struct Buffer
	{
		bool valid() const { return id >= 0; }

		static constexpr int Invalid = -1;
		int id = -1;
	};

	class Device
	{
	public:
		virtual RenderQueue& renderQueue() = 0;

		// Texture sampler
		virtual TextureSampler createTextureSampler(const TextureSampler::Descriptor&) = 0;
		virtual void destroyTextureSampler(TextureSampler) = 0;

		// Texture
		virtual Texture2d createTexture2d(const Texture2d::Descriptor&) = 0;
		virtual void destroyTexture2d(Texture2d) = 0;

		// Frame buffers
		virtual FrameBuffer createFrameBuffer(const FrameBuffer::Descriptor&) = 0;

		// Render passes
		virtual RenderPass* createRenderPass(const RenderPass::Descriptor&) = 0;
		virtual void destroyRenderPass(const RenderPass&) = 0;

		// Pipeline
		virtual Pipeline::ShaderModule createShaderModule(const Pipeline::ShaderModule::Descriptor&) = 0;
		virtual Pipeline createPipeline(const Pipeline::Descriptor&) = 0;

		// Buffers
		virtual Buffer allocateStaticVtxBuffer(size_t byteSize, const void* data) = 0;
		virtual Buffer allocateIndexBuffer(size_t byteSize, const void* data) = 0;

	protected:
		Device() = default;
	};
}
