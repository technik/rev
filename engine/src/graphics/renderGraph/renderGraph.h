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
#include <string>
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
		static constexpr unsigned MaxPasses = 32;

		~RenderGraph() { reset(); }

		// Render resources
		struct DepthRT
		{
			uint32_t id;
		};

		struct ColorRT
		{
			uint32_t id;
		};

		class PassBuilder
		{
		public:
			virtual void clear(DepthRT&, float clearValue) = 0;
			virtual void write(DepthRT&) = 0;
			virtual void read(DepthRT) = 0;

			virtual void clear(ColorRT&, const math::Vec4f& clearValue) = 0;
			virtual void write(uint32_t bindSlot, ColorRT&) = 0;
		};

		using PassConstructionCb = std::function<void(PassBuilder&)>;
		using PassExecutionCb = std::function<void(CommandList&)>;

		// Resources
		DepthRT createDepthRT(const math::Vec2u& size);
		ColorRT importRT(GpuBuffer& buffer, RenderTargetView& rt, CommandList::ResourceState defaultState);

		// Render passes
		void addPass(std::string_view passName, const PassConstructionCb&, const PassExecutionCb&);

		// RenderGraph lifetime
		void reset();
		void compile();
		void record(CommandList& cmdList); // Optionally submit command lists on the fly as they are recorded?

		// Imgui based debug interface
		void drawGraph() {}

	private:
		GpuBuffer* m_gpuBuffers[MaxBuffers] = {};
		RenderTargetView* m_renderTargets[MaxBuffers] = {};
		uint32_t m_numBuffers = 0;

		struct PassDesc
		{
			std::string name;
			PassConstructionCb constructionCb;
			PassExecutionCb executionCb;

			// TODO: Break this struct into two: desc + built
			bool clearZ = false;
			bool clearColor = false;
			float zValue = 0.f;
			math::Vec4f color;

			uint32_t colorAttach = uint32_t(-1);
			uint32_t depthAttach = uint32_t(-1);
		};

		class PassBuilderImpl : public PassBuilder
		{
		public:
			void clear(DepthRT&, float clearValue) override {}
			void write(DepthRT&) override {}
			void read(DepthRT) override {}

			void clear(ColorRT&, const math::Vec4f& clearValue) override {}
			void write(uint32_t bindSlot, ColorRT&) override {}

			PassDesc* currentPass = nullptr;
		};

		PassDesc m_passes[MaxPasses];
		uint32_t m_numPasses = 0;
	};

}
