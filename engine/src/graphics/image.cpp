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
#include "Image.h"
#include <cstdint>
#include <core/platform/fileSystem/file.h>
#include <stb_image.h>

namespace rev::gfx
{
	//----------------------------------------------------------------------------------------------
	Image::Image(PixelFormat pxlFormat, const math::Vec2u& size)
		: mSize(size)
		, mFormat(pxlFormat)
	{
		mCapacity = area();
		mData = allocatePixelData(pxlFormat, mCapacity);
	}

	//----------------------------------------------------------------------------------------------
	Image::Image(const Image& src)
		: mSize(src.mSize)
		, mCapacity(src.area())
		, mFormat(src.mFormat)
	{
		auto memorySize = rawDataSize();
		mData = new uint8_t[memorySize];
		memcpy(mData, src.mData, memorySize);
	}

	//----------------------------------------------------------------------------------------------
	Image::Image(Image&& x)
		: mSize(x.mSize)
		, mCapacity(x.mCapacity)
		, mFormat(x.mFormat)
	{
		auto memorySize = mFormat.pixelSize() * mCapacity;
		mData = x.mData;
		x.mCapacity = 0;
	}

	//----------------------------------------------------------------------------------------------
	// Constructors from specific color formats
	Image::Image(const math::Vec2u& size, math::Vec3u8* data)
		: Image(size, data, {ChannelFormat::Byte, 3})
	{}

	//----------------------------------------------------------------------------------------------
	Image::Image(const math::Vec2u& size, math::Vec4u8* data)
		: Image(size, data, {ChannelFormat::Byte, 4})
	{}

	//----------------------------------------------------------------------------------------------
	Image::Image(const math::Vec2u& size, math::Vec3f* data)
		: Image(size, data, {ChannelFormat::Float32, 3})
	{}

	//----------------------------------------------------------------------------------------------
	Image::Image(const math::Vec2u& size, math::Vec4f* data)
		: Image(size, data, {ChannelFormat::Float32, 4})
	{}

	//----------------------------------------------------------------------------------------------
	Image::~Image()
	{
		if(mCapacity)
			delete[] mData;
	}

	//----------------------------------------------------------------------------------------------
	Image& Image::operator=(const Image& x)
	{
		delete[] mData;
		mSize = x.size();
		mCapacity = area();
		mFormat = x.format();
		mData = allocatePixelData(mFormat, mCapacity);
		assert(x.mCapacity >= x.area());
		memcpy(mData, x.mData, mCapacity);
	}

	//----------------------------------------------------------------------------------------------
	Image& Image::operator=(Image&& x)
	{
		delete[] mData;
		mSize = x.size();
		mCapacity = x.mCapacity;
		mFormat = x.format();
		mData = x.mData;
		x.mCapacity = 0;
	}

	//----------------------------------------------------------------------------------------------
	void Image::clear()
	{
		mSize = math::Vec2u::zero();
	}

	//----------------------------------------------------------------------------------------------
	void Image::erase()
	{
		clear();
		if(mCapacity)
			delete[] mData;
		mCapacity = 0;
	}

	//----------------------------------------------------------------------------------------------
	void Image::resize(const math::Vec2u& size)
	{
		mSize = size;
		if(area() > mCapacity)
		{
			delete[] mData;
			mCapacity = area();
			mData = allocatePixelData(mFormat, mCapacity);
		}
	}

	//----------------------------------------------------------------------------------------------
	void Image::setPixelFormat(PixelFormat fmt)
	{
		if(mCapacity > 0 && fmt.pixelSize() != mFormat.pixelSize())
			mCapacity = mCapacity * mFormat.pixelSize() / fmt.pixelSize();
		mFormat = fmt;
	}

	//----------------------------------------------------------------------------------------------
	Image Image::proceduralXOR(const math::Vec2u& size, size_t nChannels)
	{
		Image xor({ChannelFormat::Byte, nChannels}, size);
		for(unsigned i = 0; i < size.y(); ++i)
			for(unsigned j = 0; j < size.x(); ++j)
			{
				auto clr = uint8_t(i^j);
				auto pixelNdx = xor.indexFromPos({j,i});
				for(uint8_t k = 0; k < nChannels; ++k)
				{
					auto dataOffset = k + pixelNdx*nChannels;
					xor.data<uint8_t>()[dataOffset] = clr;
				}
			}
		return xor;
	}

	//----------------------------------------------------------------------------------------------
	Image Image::load(std::string_view _name, unsigned nChannels)
	{
		core::File file(&_name[0]); //<-- Hack!
		if(file.sizeInBytes() > 0)
		{
			bool isHDR = stbi_is_hdr_from_memory((uint8_t*)file.buffer(), file.sizeInBytes());
			int width, height, realNumChannels;
			uint8_t* imgData;
			// Read image data from buffer
			if(isHDR)
				imgData = (uint8_t*)stbi_loadf_from_memory((const uint8_t*)file.buffer(), file.sizeInBytes(), &width, &height, &realNumChannels, nChannels);
			else
				imgData= stbi_load_from_memory((const uint8_t*)file.buffer(), file.sizeInBytes(), &width, &height, &realNumChannels, nChannels);

			// Create the actual image
			if(imgData)
			{
				math::Vec2u size = { unsigned(width), unsigned(height)};
				PixelFormat format;
				format.numChannels = nChannels?nChannels:(unsigned)realNumChannels;
				format.channel = isHDR ? ChannelFormat::Float32 : ChannelFormat::Byte;
				return Image(format, size, imgData);
			}
		}

		return Image(PixelFormat{ChannelFormat::Byte, 0}); // Invalid image
	}

	//----------------------------------------------------------------------------------------------
	Image::Image(PixelFormat pxlFmt, const math::Vec2u& size, void* data)
		: mSize(size)
		, mFormat(pxlFmt)
		, mData(data)
	{
		mCapacity = area();
	}

	//----------------------------------------------------------------------------------------------
	void* Image::allocatePixelData(PixelFormat pxlFormat, size_t numPixels)
	{
		auto rawSize = numPixels * pxlFormat.pixelSize();
		return new uint8_t[rawSize];
	}

	//----------------------------------------------------------------------------------------------
	size_t Image::indexFromPos(const math::Vec2u& p) const
	{
		return p.x() + p.y() * mSize.x();
	}

	//----------------------------------------------------------------------------------------------
	size_t Image::rawDataSize() const
	{
		return mFormat.pixelSize() * mCapacity;
	}
}