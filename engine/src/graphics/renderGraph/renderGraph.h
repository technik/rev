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

#include "../backend/namedResource.h"
#include "types.h"

#include <graphics/backend/gpuTypes.h>
#include <graphics/backend/texture2d.h>
#include <math/algebra/vector.h>

#include <functional>
#include <map>

namespace rev::gfx {

	class CommandBuffer;
	class Device;

	class RenderGraph
	{
	public:

		struct BufferResource : NamedResource {
			BufferResource() = default;
			BufferResource(int id) : NamedResource(id) {}
		};

		// Pass building interface
		struct IPassBuilder
		{
			virtual ~IPassBuilder() {}
			virtual BufferResource write(FrameBuffer externalTarget) = 0; // Import external frame buffer into the graph
			// TODO: virtual BufferResource write(Texture) = 0; // Import external texture to use as an output to the graph. Useful for tool writing
			virtual BufferResource write(BufferFormat) = 0;
			virtual BufferResource write(BufferResource) = 0;
			virtual void read(BufferResource, int bindingPos) = 0;

			static constexpr size_t cMaxInputs = 4;
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
		void sortPasses(); // Sort passes based on their dependencies

	private:
		Device& m_gfxDevice;

		struct PassState
		{
			size_t virtualBufferNdx;
			unsigned writeCounter; // Increased everytime a pass writes to this buffer
		};

		struct VirtualResource
		{
			FrameBuffer externalFramebuffer; // Optional imported frame buffer
			Texture2d externalTexture; // Optional imported texture
			BufferDesc bufferDescriptor; // Descriptor for non-imported targets
		};

		// Keeps track of pass info during the construction build phase of the graph
		struct PassBuilder : IPassBuilder
		{
			PassBuilder(std::vector<PassState>& bufferLifetime, std::vector<VirtualResource>& virtualResources)
				: m_bufferLifetime(bufferLifetime)
				, m_virtualResources(virtualResources)
			{}

			BufferResource write(FrameBuffer externalTarget) override; // Import external frame buffer into the graph
			BufferResource write(BufferFormat) override;
			BufferResource write(BufferResource) override;
			void read(BufferResource, int bindingPos) override;

			// References to the rendergraph´s buffer state
			std::vector<PassState>& m_bufferLifetime;
			std::vector<VirtualResource>& m_virtualResources;

			// Dependencies
			std::vector<size_t> m_inputs; // Indices into m_bufferLifetime
			std::vector<size_t> m_outputs; // Indices into m_bufferLifetime

			// Pass description
			math::Vec2u targetSize; // Texture size of all attachments written to during the pass
			HWAntiAlias antiAliasing;
			PassDefinition definition;
			PassEvaluator evaluator;

		private:
			BufferResource registerOutput(PassState);
		};

		// Passes
		std::vector<PassBuilder> m_passDescriptors;
		// Map of buffers and life cycle counter, used during build phase (and potentially during evaluation for sanity checks)
		// Buffer resource handles returned to the user are actually indices into this vector.
		// Each entry contains an index into the virtual buffers array, and a counter that represents the number of write
		// subpasses that buffer has gone through until this point.
		std::vector<PassState> m_bufferLifetime;

		// Internal state
		// Indices into m_passDescriptors, sorted such that dependencies are satisfied.
		std::vector<size_t> m_sortedPasses;

		// Resources
		std::vector<VirtualResource> m_virtualResources;
	};

}
 