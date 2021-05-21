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
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "Image.h"
#include <cstdint>
#include <core/platform/fileSystem/file.h>
#include <math/linear.h>

namespace rev::gfx
{
	namespace
	{
		auto extension(const std::string_view& s)
		{
			auto dot = s.find_last_of(".");
			if (dot == std::string_view::npos)
				return std::string_view("");
			else
				return s.substr(dot + 1);
		}
	}

	vk::Format Image::GetPixelFormat(bool hdr, unsigned numChannels, bool srgb)
	{
		if (hdr)
		{
			assert(!srgb);
			switch (numChannels)
			{
			case 1:
				return vk::Format::eR32Sfloat;
			case 2:
				return vk::Format::eR32G32Sfloat;
			case 3:
				return vk::Format::eR32G32B32Sfloat;
			case 4:
				return vk::Format::eR32G32B32A32Sfloat;
			}
		}
		else
		{
			assert(!srgb || numChannels > 2);
			switch (numChannels)
			{
			case 1:
				return vk::Format::eR8Unorm;
			case 2:
				return vk::Format::eR8G8Unorm;
			case 3:
				return srgb ? vk::Format::eR8G8B8Srgb : vk::Format::eR8G8B8Unorm;
			case 4:
				return srgb ? vk::Format::eR8G8B8A8Srgb : vk::Format::eR8G8B8A8Unorm;
			default:
				assert(false);
				return vk::Format::eR8G8B8A8Unorm;
			}
		}
	}

	//----------------------------------------------------------------------------------------------
	Image::Image(vk::Format pxlFormat, const math::Vec2u& size)
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
		auto memorySize = GetPixelSize(mFormat) * mCapacity;
		mData = x.mData;
		x.mCapacity = 0;
	}

	//----------------------------------------------------------------------------------------------
	// Constructors from specific color formats
	Image::Image(const math::Vec2u& size, math::Vec3u8* data)
		: Image(vk::Format::eR8G8B8Unorm, size, data)
	{}

	//----------------------------------------------------------------------------------------------
	Image::Image(const math::Vec2u& size, math::Vec4u8* data)
		: Image(vk::Format::eR8G8B8A8Unorm, size, data)
	{}

	//----------------------------------------------------------------------------------------------
	Image::Image(const math::Vec2u& size, math::Vec3f* data)
		: Image(vk::Format::eR32G32B32Sfloat, size, data)
	{}

	//----------------------------------------------------------------------------------------------
	Image::Image(const math::Vec2u& size, math::Vec4f* data)
		: Image(vk::Format::eR32G32B32A32Sfloat, size, data)
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
	void Image::setPixelFormat(vk::Format fmt)
	{
		if(mCapacity > 0 && GetPixelSize(fmt) != GetPixelSize(mFormat))
			mCapacity = mCapacity * GetPixelSize(mFormat) / GetPixelSize(fmt);
		mFormat = fmt;
	}

	//----------------------------------------------------------------------------------------------
	Image Image::proceduralXOR(const math::Vec2u& size, size_t nChannels)
	{
		vk::Format format = GetPixelFormat(false, nChannels, false);
		Image xorImg(format, size);
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
	std::shared_ptr<Image> Image::load(std::string_view _name, unsigned nChannels, bool srgb)
	{
		core::File file(_name.data());
		return loadFromMemory(file.buffer(), file.size(), nChannels, srgb);
	}

	//----------------------------------------------------------------------------------------------
	std::shared_ptr<Image> Image::loadFromMemory(const void* data, size_t bufferSize, unsigned nChannels, bool srgb)
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
				vk::Format format = GetPixelFormat(isHDR, nChannels, srgb);
				auto result = std::make_shared<Image>(format, size);
				memcpy(result->data<void>(), imgData, result->area() * GetPixelSize(format));

				stbi_image_free(imgData);

				return std::move(result);
			}
		}

		return nullptr;
	}

	//----------------------------------------------------------------------------------------------
	Image::Image(vk::Format pxlFmt, const math::Vec2u& size, void* data)
		: mSize(size)
		, mFormat(pxlFmt)
		, mData(data)
	{
		mCapacity = area();
	}

	//----------------------------------------------------------------------------------------------
	void* Image::allocatePixelData(vk::Format pxlFormat, size_t numPixels)
	{
		auto rawSize = numPixels * GetPixelSize(pxlFormat);
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
		return GetPixelSize(mFormat) * mCapacity;
	}



	void saveHDR(const Image& img, const std::string& fileName)
	{
		if (extension(fileName) != "hdr")
		{
			std::cout << "Only .hdr is supported for hdr output images\n";
		}

		assert(img.format() == vk::Format::eR32G32B32Sfloat);
		auto src = reinterpret_cast<const float*>(img.data());
		stbi_write_hdr(fileName.c_str(), img.width(), img.height(), 3, src);
	}

	void save2sRGB(const Image& img, const std::string& fileName)
	{
		if (extension(fileName) != "png")
		{
			std::cout << "Only png is supported for output images\n";
		}
		auto nBytes = 3 * img.area();
		std::vector<uint8_t> raw(nBytes);
		assert(img.format() == vk::Format::eR32G32B32Sfloat);
		auto src = reinterpret_cast<const float*>(img.data());

		// Linear to sRGB conversion following the formal specification of sRGB
		for (int i = 0; i < nBytes; ++i)
		{
			auto linear = src[i];
			float srgb = pow(linear, 0.4545f) * 255;
			/*if(linear <= 0.0031308f)
			{
				srgb = clamp(linear * 3294.6f, 0.f, 255.f);
			}
			else
			{
				srgb = 269.025f * pow(linear,1/2.4f) - 0.055f;
			}*/
			raw[i] = uint8_t(math::clamp(srgb + 0.5f, 0.f, 255.f));
		}
		auto bytesPerRow = 3 * img.width();
		stbi_write_png(fileName.c_str(), img.width(), img.height(), 3, raw.data(), bytesPerRow);
	}

	void saveLinear(const Image& img, const std::string& fileName)
	{
		if (extension(fileName) != "png")
		{
			std::cout << "Only png is supported for output images\n";
		}
		auto nBytes = 3 * img.area();
		std::vector<uint8_t> raw(nBytes);
		assert(img.format() == vk::Format::eR32G32B32Sfloat);
		auto src = reinterpret_cast<const float*>(img.data());

		// Linear to sRGB conversion following the formal specification of sRGB
		for (int i = 0; i < nBytes; ++i)
		{
			auto linear = src[i] * 255.f;
			raw[i] = (uint8_t)math::clamp(linear, 0.f, 255.f);
		}
		auto bytesPerRow = 3 * img.width();
		stbi_write_png(fileName.c_str(), img.width(), img.height(), 3, raw.data(), bytesPerRow);
	}
}