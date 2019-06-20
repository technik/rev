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

#include <functional>
#include <string_view>

#include <graphics/backend/commandList.h>
#include <math/algebra/vector.h>

namespace rev::gfx {

	class CommandList;
	class GpuBuffer;
	class RenderTargetView;

	class RenderGraph
	{
	public:
		static constexpr unsigned MaxBuffers = 32;

		// Render resources
		struct DepthRT
		{
			uint32_t id;
		};

		struct ColorRT
		{
			uint32_t id;
		};

		class Pass
		{
		public:
			void clear(DepthRT&, float clearValue);
			void write(DepthRT&);
			void read(DepthRT);

			void clear(ColorRT&, const math::Vec4f& clearValue);
			void write(uint32_t bindSlot, ColorRT&);

		private:
		};

		using PassExecutionCb = std::function<void(CommandList&)>;
		using PassConstructionCb = std::function<void(Pass&)>;

		// Resources
		DepthRT createDepthRT(const math::Vec2u& size);
		ColorRT importRT(const GpuBuffer& buffer, const RenderTargetView& rt, CommandList::ResourceState defaultState);

		// Render passes
		void addPass(std::string_view passName, const PassConstructionCb&, const PassExecutionCb&);

		// RenderGraph lifetime
		void reset();
		void compile();
		void record(); // Optionally submit command lists on the fly as they are recorded

		// Imgui based debug interface
		void drawGraph();

	private:
		GpuBuffer* m_gpuBuffers[MaxBuffers];
		RenderTargetView* m_renderTargets[MaxBuffers];
	};

}
