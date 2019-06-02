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
		RenderGraph(Device&);
		// Graph lifetime
		void reset();
		void compile();
		// Record graph execution into a command buffer for deferred submision
		void recordExecution(CommandBuffer& dst);
		void run(); // Execute graph on the fly, submitting command buffers as they are ready
		void clearResources(); // Free allocated memory resources

		// Resources
		enum class DepthFormat
		{
			f24,
			f32
		};

		struct Attachment : NamedResource {
			Attachment() = default;
			Attachment(int id) : NamedResource(id) {}
		};

		// Graph building
		enum class ReadMode
		{
			clear,
			keep,
			discard
		};

		enum HWAntiAlias
		{
			none,
			msaa2x,
			msaa4x,
			msaa8x
		};

		struct FrameBufferOptions
		{
			math::Vec2u size;
		};

		struct Pass : NamedResource {
			Pass() = default;
			Pass(int id) : NamedResource(id) {}
		};

		Pass pass(const math::Vec2u& size, HWAntiAlias);
		void readColor(Pass, int bindingLocation, Attachment);
		void readDepth(Pass, int bindingLocation, Attachment);
		// By default, write to a new resource
		Attachment writeColor(Pass, PixelFormat, int bindingLocation, ReadMode, Attachment = Attachment());
		Attachment writeDepth(Pass, DepthFormat, ReadMode, Attachment = Attachment());

		using PassExecution = std::function<void(const RenderGraph& rg, CommandBuffer& dst)>;
		template<class PassExec>
		void setExecution(Pass pass, PassExec& exec ) { m_renderPasses[pass.id()].exec = exec; }

		// Resource access during execution
		Texture2d getTexture(Attachment) const;

	private:
		Device& m_device;
		int m_nextResourceId = 0;

		struct WriteAttachment
		{
			ReadMode m_readMode;
			Attachment m_beforeWrite;
			Attachment m_afterWrite;
		};

		struct RenderPassInfo
		{
			std::map<int, Attachment> m_colorInputs;
			std::map<int, Attachment> m_depthInputs;
			std::map<int, WriteAttachment> m_colorOutputs;
			WriteAttachment m_depthOutput;

			math::Vec2u m_targetSize;
			PassExecution m_execution;
			HWAntiAlias m_antiAlias;

			FrameBuffer m_targetFB;
		};

		std::vector<RenderPassInfo> m_renderPasses;
		std::map<int32_t,Texture2d> m_resolvedTextures;
		TextureSampler m_linearSampler;
	};

}
