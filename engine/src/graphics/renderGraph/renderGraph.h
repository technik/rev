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
#include <math/algebra/vector.h>

namespace rev::gfx {

	class RenderGraph
	{
	public:
		// Graph lifetime
		void reset();
		void bake();
		void execute();
		void clearResources(); // Free allocated memory resources

		// Resources
		enum class ColorFormat
		{
			None,
			RGBA8,
			sRGBA8,
			RGBA32
		};

		enum class DepthFormat
		{
			None,
			f24,
			f32
		};

		struct Attachment : NamedResource {};

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

		struct FrameBuffer : NamedResource {};

		FrameBuffer frameBuffer(const math::Vec2u& size, HWAntiAlias);
		Attachment readColor(int bindingLocation, Attachment);
		Attachment readDepth(Attachment);
		// By default, write to a new resource
		Attachment writeColor(FrameBuffer, int bindingLocation, ReadMode, Attachment = Attachment());
		Attachment writeDepth(FrameBuffer, ReadMode, Attachment = Attachment());

	};

}
