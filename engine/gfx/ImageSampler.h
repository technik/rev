//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2021 Carmelo J Fdez-Aguera
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

#include <gfx/Image.h>
#include <math/algebra/vector.h>
#include <math/linear.h>

namespace rev::gfx
{
	template<class T> struct ImageSampler;

	template<class T, size_t N>
	struct ImageSampler<math::Vector<T, N>>
	{
		using texel = math::Vector<T, N>;
		std::shared_ptr<const Image<T, N>> m_image;

		texel sample(const math::Vec2f& uv) const
		{
			math::Vec2f scaled = m_image->size().cast<float>() * uv;
			math::Vec2f centered = scaled - 0.5f;
			math::Vec2i uv0 = centered.cast<int32_t>();
			math::Vec2f alpha = centered - uv0.cast<float>();
			math::Vec2i uv1 = clamp(math::Vec2i(uv0 + 1), math::Vec2i(0), m_image->size().cast<int32_t>()-1);
			uv0 = clamp(uv0, math::Vec2i(0), m_image->size().cast<int32_t>());

			texel top = m_image->pixel(uv0.cast<uint32_t>()) * (1 - alpha.x()) + alpha.x() * m_image->pixel(uv1.x(), uv0.y());
			texel bottom = m_image->pixel(uv0.x(), uv1.y()) * (1 - alpha.x()) + alpha.x() * m_image->pixel(uv1.cast<uint32_t>());
			return top * (1 - alpha.y()) + alpha.y() * bottom;
		}
	};
}