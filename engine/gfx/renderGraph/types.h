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

// rev::math
#include <math/algebra/vector.h>

// Std library
#include <optional>

namespace rev::gfx {

	enum class BufferFormat
	{
		R8,
		RGBA8,
		sRGBA8,
		RGBA32,
		depth24,
		depth32
	};

	enum class HWAntiAlias
	{
		none,
		msaa2x,
		msaa4x,
		msaa8x
	};

	struct BufferDesc
	{
		math::Vec2u size;
		BufferFormat format;
		HWAntiAlias antiAlias;

		bool operator==(const BufferDesc& other) const
		{
			return size == other.size
				&& format == other.format
				&& antiAlias == other.antiAlias;
		}
	};

}