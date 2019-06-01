//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2019 Carmelo J Fdez-Aguera
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
#include <cstdint>
#include <math/algebra/vector.h>

namespace rev::gfx {

	class CommandPool;
	class Fence;
	class GpuBuffer;
	class Pipeline;
	class RenderTargetView;

	class CommandList
	{
	public:
		enum Type
		{
			Graphics,
			Compute,
			Copy,
			Bundle
		};

		enum class Barrier
		{
			Transition
		};

		enum class ResourceState
		{
			RenderTarget,
			PixelShaderResource,
			CopySource,
			CopyDst,
			GenericRead,
			Present
		};

		enum NdxBufferFormat
		{
			U8,
			U16,
			U32
		};

		virtual void reset(CommandPool& cmdPool) = 0;
		virtual void close() = 0;

		// Commands
		virtual void resourceBarrier(GpuBuffer* resource, Barrier barrierType, ResourceState before, ResourceState after) = 0;
		virtual void clearRenderTarget(RenderTargetView* rt, math::Vec4f color) = 0;

		virtual void bindPipeline(const Pipeline*) = 0;
		virtual void bindAttribute(int binding, int sizeInBytes, int stride, GpuBuffer*) = 0;
		virtual void bindIndexBuffer(int sizeInBytes, NdxBufferFormat, GpuBuffer*) = 0;
		virtual void bindRenderTarget(RenderTargetView* rt) = 0;

		virtual void drawIndexed(int indexOffset, int indexCount, int vertexOffset) = 0;

		virtual void setViewport(const math::Vec2u& pos, const math::Vec2u& size) = 0;
		virtual void setScissor(const math::Vec2u& pos, const math::Vec2u& size) = 0;

		virtual void setConstants(uint32_t regIndex, uint32_t size, const void* data) = 0;

		virtual void uploadBufferContent(const GpuBuffer& dst, const GpuBuffer& stagingBuffer, size_t dataSize, const void* data) = 0;
	};
}
