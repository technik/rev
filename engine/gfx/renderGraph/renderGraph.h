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

#include <math/algebra/vector.h>

#include <functional>
#include <map>
#include <string>

namespace rev::gfx {

	class FrameBufferCache;

	/*class RenderGraph
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
			// Import external frame buffer into the graph.
			virtual BufferResource write(FrameBuffer externalTarget) = 0;
			// Import external texture to use as an output to the graph. Useful for tool writing.
			virtual BufferResource write(Texture2d) = 0;
			// Import one side of a cubemap as an output texture.
			virtual BufferResource write(Texture2d, Texture2d::CubeMapSide) = 0;
			// Write to a new buffer defined by the given format.
			virtual BufferResource write(BufferFormat) = 0;
			// Write to a buffer from a previous pass.
			virtual BufferResource write(BufferResource) = 0;
			virtual void read(BufferResource, int bindingPos) = 0;

			static constexpr size_t cMaxInputs = 8;
		};

		using PassDefinition = std::function<void(IPassBuilder&)>;
		using PassEvaluator = std::function<void(const Texture2d* inputTextures, size_t nInputTextures, CommandBuffer& dst)>;

	public:

		RenderGraph(Device&);

		// Graph lifetime
		void reset(); // Does not clear allocated GPU resources.
		void addPass(const std::string& name, const math::Vec2u& size, PassDefinition, PassEvaluator, HWAntiAlias = HWAntiAlias::none);
		void build(FrameBufferCache&);

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
			Texture2d::CubeMapSide cubemapSide = Texture2d::CubeMapSide::None;
			BufferDesc bufferDescriptor; // Descriptor for non-imported targets
		};

		// Keeps track of pass info during the construction build phase of the graph
		struct PassBuilder : IPassBuilder
		{
			PassBuilder(const std::string& passName, std::vector<PassState>& bufferLifetime, std::vector<VirtualResource>& virtualResources)
				: m_bufferLifetime(bufferLifetime)
				, m_virtualResources(virtualResources)
				, name(passName)
			{}

			BufferResource write(FrameBuffer externalTarget) override; // Import external frame buffer into the graph
			BufferResource write(Texture2d externalTexture) override;
			BufferResource write(Texture2d externalCubemapTexture, Texture2d::CubeMapSide) override;
			BufferResource write(BufferFormat) override;
			BufferResource write(BufferResource) override;
			void read(BufferResource, int bindingPos) override;

			// References to the rendergraph�s buffer state
			std::vector<PassState>& m_bufferLifetime;
			std::vector<VirtualResource>& m_virtualResources;

			// Dependencies
			std::vector<size_t> m_inputs; // Indices into m_bufferLifetime
			std::vector<size_t> m_outputs; // Indices into m_bufferLifetime

			// Pass description
			std::string name;
			math::Vec2u targetSize; // Texture size of all attachments written to during the pass
			HWAntiAlias antiAliasing;
			PassDefinition definition;
			PassEvaluator evaluator;

			// Cached state
			FrameBuffer m_cachedFb;

		private:
			BufferResource registerOutput(PassState);
		};

		static constexpr size_t cMaxOutputs = 8; // Max number of outputs in a single pass. Including a possible depth buffer.

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
		std::map<size_t, Texture2d> m_virtualToPhysical; // Mapping from virtual resource indices to frame buffer attachments
	};*/

}
 