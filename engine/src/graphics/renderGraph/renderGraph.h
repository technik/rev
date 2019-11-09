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

#include "../backend/namedResource.h"
#include <functional>
#include <graphics/backend/frameBuffer.h>
#include <graphics/backend/texture2d.h>
#include <map>
#include <math/algebra/vector.h>

namespace rev::gfx {

	class CommandBuffer;
	class Device;

	class RenderGraph
	{
	public:

		enum class ColorFormat
		{
			RGBA8,
			sRGBA8,
			RGBA32
		};

		enum class DepthFormat
		{
			f24,
			f32
		};

		struct BufferResource : NamedResource {
			BufferResource() = default;
			BufferResource(int id) : NamedResource(id) {}
		};

		enum class HWAntiAlias
		{
			none,
			msaa2x,
			msaa4x,
			msaa8x
		};

		// Pass building interface
		struct IPassBuilder
		{
			virtual ~IPassBuilder() {}
			virtual BufferResource write(FrameBuffer) = 0; // Import external frame buffer into the graph
			// TODO: virtual BufferResource write(Texture) = 0; // Import external texture to use as an output to the graph. Useful for tool writing
			virtual BufferResource write(DepthFormat) = 0;
			virtual BufferResource write(ColorFormat) = 0;
			virtual BufferResource write(BufferResource) = 0;
			virtual void read(BufferResource, int bindingPos) = 0;
			virtual void modify(BufferResource) = 0;
		};

		using PassDefinition = std::function<void(IPassBuilder&)>;
		using PassEvaluator = std::function<void(const Texture2d* inputTextures, size_t nInputTextures, CommandBuffer& dst)>;

	public:

		RenderGraph(Device&);

		// Graph lifetime
		void reset(); // Does not clear allocated GPU resources.
		void addPass(const math::Vec2u& size, PassDefinition, PassEvaluator, HWAntiAlias = HWAntiAlias::none);
		void build();

		// Record graph execution into a command buffer for deferred submision
		void evaluate(CommandBuffer& dst);

		// Free allocated memory resources, like textures and frame buffers. Must not be called on a built graph
		void clearResources();

	private:
		Device& m_gfxDevice;

		struct RenderPassDescriptor
		{
			PassDefinition definition;
			PassEvaluator evaluator;
			math::Vec2u targetSize; // Size of all attachments written to during the pass
			HWAntiAlias antiAliasing;
		};

		// Keeps track of pass info during the construction build phase of the graph
		struct PassBuilder : IPassBuilder
		{
			PassBuilder(std::vector<std::pair<size_t, int>>& bufferState) : m_buffersState(bufferState) {}

			BufferResource write(FrameBuffer) override; // Import external frame buffer into the graph
			BufferResource write(DepthFormat) override;
			BufferResource write(ColorFormat) override;
			BufferResource write(BufferResource) override;
			void read(BufferResource, int bindingPos) override;
			void modify(BufferResource) override;

			std::vector<std::pair<size_t, int>>& m_buffersState;
			math::Vec2u targetSize; // Size of all attachments written to during the pass
		};

		// Passes
		// Map of buffers and life cycle counter, used during build phase (and potentially during evaluation for sanity checks
		// Buffer resource indices returned to the user are actually indices into this vector.
		// Each entry contains an index into the buffer attachments array, and a counter that represents the number of write
		// subpasses that buffer has gone through until this point
		std::vector<std::pair<size_t, int>> m_buffersState;
		std::vector<RenderPassDescriptor> m_passDescriptors;

		// Resources
		std::vector<FrameBuffer> m_buffers;
		std::vector<Texture2d> m_textures;
		std::vector<FrameBuffer::Attachment> m_bufferAttachments;
		std::vector<FrameBuffer::Descriptor> m_bufferDescriptors;
	};

}
