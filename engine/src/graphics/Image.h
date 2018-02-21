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

#include <core/platform/fileSystem/file.h>
#include <math/algebra/vector.h>
#include "stb_image.h"
#include <memory>
#include <string>

namespace rev { namespace graphics {

	class Image
	{
	public:
		Image(const math::Vec2u& size, std::shared_ptr<uint8_t> data, unsigned nChannels)
			: mNumChannels(nChannels)
			, mSize(size)
			, mData(data)
		{}

		// XOR textures are always 8-bits per channel
		static Image proceduralXOR(unsigned size, unsigned nChannels)
		{
			auto imgBuffer = std::shared_ptr<uint8_t>( new uint8_t[size*size*nChannels], []( uint8_t *p ){ delete [] p; } );
			for(unsigned i = 0; i < size; ++i)
				for(unsigned j = 0; j < size; ++j)
					for(uint8_t k = 0; k < nChannels; ++k)
					{
						auto pixelNdx = i+j*size;
						auto dataOffset = k + pixelNdx*nChannels;
						imgBuffer.get()[dataOffset] = uint8_t(i^j);
					}
			return Image({size,size}, imgBuffer, nChannels);
		}

		// Note: nChannels = 0 sets automatic number of channels
		static std::shared_ptr<Image> load(const std::string& _name, unsigned nChannels)
		{
			core::File file(_name);
			if(file.sizeInBytes() > 0)
			{
				int width, height, realNumChannels;
				auto imgData = stbi_load_from_memory((const uint8_t*)file.buffer(), file.sizeInBytes(), &width, &height, &realNumChannels, nChannels);
				if(imgData)
				{
					math::Vec2u size = { unsigned(width), unsigned(height)};
					return std::make_shared<Image>(size, std::shared_ptr<uint8_t>(imgData), nChannels?nChannels:realNumChannels);
				}
			}

			return nullptr;
		}

		// Accessors
		unsigned				nChannels() const { return mNumChannels; };
		const math::Vec2u&		size() const { return mSize; }
		template<typename T>
		const T*				data() const { return reinterpret_cast<T*>(mData.get()); }
		const uint8_t*			data() const { return mData.get(); }

	private:
		unsigned					mNumChannels;
		math::Vec2u					mSize;
		std::shared_ptr<uint8_t>	mData;
	};

}}
