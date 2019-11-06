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

		enum HWAntiAlias
		{
			none,
			msaa2x,
			msaa4x,
			msaa8x
		};

		// Pass building interface
		struct PassBuilder
		{
			virtual BufferResource write(FrameBuffer) = 0; // Import external frame buffer into the graph
			// TODO: virtual BufferResource write(Texture) = 0; // Import external texture to use as an output to the graph. Useful for tool writing
			virtual BufferResource write(DepthFormat) = 0;
			virtual BufferResource write(ColorFormat) = 0;
			virtual BufferResource write(BufferResource) = 0;
			virtual void read(BufferResource, int bindingPos) = 0;
			virtual void modify(BufferResource) = 0;
		};

		using PassDefinition = std::function<void(PassBuilder&)>;
		using PassEvaluator = std::function<void(CommandBuffer& dst)>;

	public:

		RenderGraph(Device&);
		// Graph lifetime
		void reset();
		void addPass(const math::Vec2u& size, PassDefinition, PassEvaluator, HWAntiAlias = HWAntiAlias::none);
		void build();

		// Record graph execution into a command buffer for deferred submision
		void evaluate(CommandBuffer& dst);

		// Free allocated memory resources. Must not be called on a built graph
		void clearResources();

	private:
		Device& m_gfxDevice;

		struct RenderPassInfo
		{
			math::Vec2u targetSize;
		};
	};

}
