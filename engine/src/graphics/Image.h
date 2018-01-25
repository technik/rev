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

#include <math/algebra/vector.h>
#include <memory>

namespace rev { namespace graphics {

	template<typename T, uint8_t nChannels>
	class Image
	{
		static_assert(nChannels <= 4, "Images with more than 4 channels are not supported");

	public:
		Image(const math::Vec2u& size, std::shared_ptr<T> data)
			: mSize(size)
			, mData(data)
		{}

		static Image proceduralXOR(unsigned size)
		{
			auto imgBuffer = std::shared_ptr<T>( new T[size*size*depth], []( T *p ){ delete [] p; } );
			for(unsigned i = 0; i < size; ++i)
				for(unsigned j = 0; j < size; ++j)
					for(uint8_t k = 0; k < depth; ++k)
					{
						auto pixelNdx = i+j*size;
						auto dataOffset = k + pixelNdx*depth;
						imgBuffer.get()[dataOffset] = T(i^j);
					}
			return Image({size,size}, imgBuffer);
		}

		// Accessors
		static constexpr uint8_t depth = nChannels;
		const math::Vec2u&	size() const { return mSize; }
		auto				data() const { return mData; }

	private:
		math::Vec2u			mSize;
		std::shared_ptr<T>	mData;
	};

	using ImageRGB8 = Image<uint8_t, 3>;
	using ImageRGBA8 = Image<uint8_t, 4>;
	using ImageRGB32F = Image<float, 3>;

}}
