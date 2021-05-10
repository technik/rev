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
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Image.h"
#include <cstdint>
#include <core/platform/fileSystem/file.h>

namespace rev::gfx
{
	//----------------------------------------------------------------------------------------------
	Image::Image(PixelFormat pxlFormat, const math::Vec2u& size)
		: mSize(size)
		, mFormat(pxlFormat)
	{
		mCapacity = area();
		if(mCapacity)
			mData = allocatePixelData(pxlFormat, mCapacity);
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
		: Image({ChannelFormat::Byte, 3}, size, data)
	{}

	//----------------------------------------------------------------------------------------------
	Image::Image(const math::Vec2u& size, math::Vec4u8* data)
		: Image({ChannelFormat::Byte, 4}, size, data)
	{}

	//----------------------------------------------------------------------------------------------
	Image::Image(const math::Vec2u& size, math::Vec3f* data)
		: Image({ChannelFormat::Float32, 3}, size, data)
	{}

	//----------------------------------------------------------------------------------------------
	Image::Image(const math::Vec2u& size, math::Vec4f* data)
		: Image({ChannelFormat::Float32, 4}, size, data)
	{}

	//----------------------------------------------------------------------------------------------
	Image::~Image()
	{
		if(mCapacity)
			delete[] mData;
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
		return *this;
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
		Image xorImg({ChannelFormat::Byte, (uint8_t)nChannels}, size);
		for(unsigned i = 0; i < size.y(); ++i)
			for(unsigned j = 0; j < size.x(); ++j)
			{
				auto clr = uint8_t(i^j);
				auto pixelNdx = xorImg.indexFromPos({j,i});
				for(uint8_t k = 0; k < nChannels; ++k)
				{
					auto dataOffset = k + pixelNdx*nChannels;
					xorImg.data<uint8_t>()[dataOffset] = clr;
				}
			}
		return xorImg;
	}

	//----------------------------------------------------------------------------------------------
	std::shared_ptr<Image> Image::load(std::string_view _name, unsigned nChannels)
	{
		core::File file(_name.data());
		return loadFromMemory(file.buffer(), file.size(), nChannels);
	}

	//----------------------------------------------------------------------------------------------
	std::shared_ptr<Image> Image::loadFromMemory(const void* data, size_t bufferSize, unsigned nChannels)
	{
		if (bufferSize > 0)
		{
			bool isHDR = stbi_is_hdr_from_memory((uint8_t*)data, bufferSize);
			int width, height, realNumChannels;
			uint8_t* imgData;
			if (!nChannels)
			{
				int srcChannels;
				stbi_info_from_memory((const uint8_t*)data, bufferSize, &width, &height, &srcChannels);
				if (srcChannels < 3)
					nChannels = 4;
			}
			// Read image data from buffer
			if (isHDR)
				imgData = (uint8_t*)stbi_loadf_from_memory((const uint8_t*)data, bufferSize, &width, &height, &realNumChannels, nChannels);
			else
				imgData = stbi_load_from_memory((const uint8_t*)data, bufferSize, &width, &height, &realNumChannels, nChannels);

			// Create the actual image
			if (imgData)
			{
				math::Vec2u size = { unsigned(width), unsigned(height) };
				PixelFormat format;
				format.numChannels = nChannels ? nChannels : (unsigned)realNumChannels;
				format.channel = isHDR ? ChannelFormat::Float32 : ChannelFormat::Byte;
				auto result = std::make_shared<Image>(format, size);
				memcpy(result->data<void>(), imgData, result->area() * format.pixelSize());

				stbi_image_free(imgData);

				return std::move(result);
			}
		}

		return nullptr;
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