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
#include <core/platform/fileSystem/fileSystem.h>
#include <core/string_util.h>
#include <math/linear.h>

using namespace rev::math;

namespace rev::gfx
{
	vk::Format Image<>::GetPixelFormat(bool hdr, unsigned numChannels, bool srgb)
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
			default:
				assert(false);
				return vk::Format::eR8G8B8A8Unorm;
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
	Image<>::Image(const Vec2u& size, vk::Format format, uint32_t byteStride, std::unique_ptr<uint8_t[]>&& data)
		: m_data(std::move(data))
		, m_size(size)
		, m_format(format)
		, m_byteStride(byteStride)
	{
	}

	//----------------------------------------------------------------------------------------------
	std::shared_ptr<Image<>> Image<>::load(std::string_view _name, unsigned nChannels, bool hdr, bool srgb)
	{
		core::File file(_name.data());
		return loadFromMemory(file.buffer(), file.size(), nChannels, hdr, srgb);
	}

	//----------------------------------------------------------------------------------------------
	template<>
	std::shared_ptr<Image<>> Image<>::loadFromMemory(const void* data, size_t size, unsigned nChannels, bool hdr, bool srgb)
	{
		if (hdr)
		{
			assert(!srgb);

			switch (nChannels)
			{
			case 3:
				return Image3f::loadFromMemory(data, size);
			case 4:
				return Image3f::loadFromMemory(data, size);
			}
		}
		else
		{
			switch (nChannels)
			{
			case 3:
				return Image3u8::loadFromMemory(data, size, srgb);
			case 4:
				return Image3u8::loadFromMemory(data, size, srgb);
			}
		}
		assert(false);
		return nullptr;
	}

	//----------------------------------------------------------------------------------------------
	Image<uint8_t,3>::Image(const Vec2u& size, bool srgb)
		: Image<void,0>(
			size,
			srgb?vk::Format::eR8G8B8Srgb : vk::Format::eR8G8B8Unorm,
			3,
			std::make_unique<uint8_t[]>(sizeof(Vec3u8)*size.x()*size.y()))
	{
	}

	//----------------------------------------------------------------------------------------------
	Image<uint8_t, 3>::Image(const Vec2u& size, std::unique_ptr<math::Vec3u8[]>&& data, bool srgb, bool stbiAlloc)
		: Image<void, 0>(
			size,
			srgb ? vk::Format::eR8G8B8Srgb : vk::Format::eR8G8B8Unorm,
			3,
			std::unique_ptr<uint8_t[]>((uint8_t*)data.release()))
		, m_isStbiAllocated(stbiAlloc)
	{
	}

	//----------------------------------------------------------------------------------------------
	Image<uint8_t, 3>::~Image()
	{
		if (m_isStbiAllocated)
		{
			stbi_image_free(m_data.release());
		}
	}

	//----------------------------------------------------------------------------------------------
	std::shared_ptr<Image<uint8_t, 3>> Image<uint8_t, 3>::proceduralXOR(const math::Vec2u& size)
	{
		auto data = std::make_unique<Vec3u8[]>(size.x() * size.y());
		auto image = std::make_unique<Image3u8>(size, std::move(data), false, false);

		for (unsigned i = 0; i < size.y(); ++i)
		{
			for (unsigned j = 0; j < size.x(); ++j)
			{
				auto clr = uint8_t(i ^ j);
				image->pixel(i, j) = Vec3u8(clr, clr, clr);
			}
		}
		return image;
	}

	//----------------------------------------------------------------------------------------------
	std::shared_ptr<Image<uint8_t, 3>> Image<uint8_t, 3>::load(std::string_view _name, bool srgb)
	{
		auto file = core::FileSystem::get()->readFile(_name.data());
		return loadFromMemory(file->buffer(), file->size(), srgb);
	}

	//----------------------------------------------------------------------------------------------
	std::shared_ptr<Image<uint8_t, 3>> Image<uint8_t, 3>::loadFromMemory(const void* data, size_t bufferSize, bool srgb)
	{
		if (bufferSize > 0)
		{
			// Process in memory data
			int width, height, realNumChannels;
			const auto expectedChannels = 3;
			uint8_t* imgData = stbi_load_from_memory((const uint8_t*)data, bufferSize, &width, &height, &realNumChannels, expectedChannels);

			// Create the actual image
			if (imgData)
			{
				auto dataPtr = std::unique_ptr<Vec3u8[]>(reinterpret_cast<Vec3u8*>(imgData));
				math::Vec2u size = { unsigned(width), unsigned(height) };
				return std::make_shared<Image3u8>(size, std::move(dataPtr), srgb, true);
			}
		}

		return nullptr;
	}

	//----------------------------------------------------------------------------------------------
	Image<uint8_t, 4>::Image(const Vec2u& size, bool srgb)
		: Image<void, 0>(
			size,
			srgb ? vk::Format::eR8G8B8A8Srgb : vk::Format::eR8G8B8A8Unorm,
			4,
			std::make_unique<uint8_t[]>(sizeof(Vec4u8)* size.x()* size.y()))
	{
	}

	//----------------------------------------------------------------------------------------------
	Image<uint8_t, 4>::Image(const Vec2u& size, std::unique_ptr<math::Vec4u8[]>&& data, bool srgb, bool stbiAlloc)
		: Image<void, 0>(
			size,
			srgb ? vk::Format::eR8G8B8A8Srgb : vk::Format::eR8G8B8A8Unorm,
			4,
			std::unique_ptr<uint8_t[]>((uint8_t*)data.release()))
		, m_isStbiAllocated(stbiAlloc)
	{
	}

	//----------------------------------------------------------------------------------------------
	Image<uint8_t, 4>::~Image()
	{
		if (m_isStbiAllocated)
		{
			stbi_image_free(m_data.release());
		}
	}

	//----------------------------------------------------------------------------------------------
	std::shared_ptr<Image4u8> Image4u8::proceduralXOR(const math::Vec2u& size)
	{
		auto data = std::make_unique<Vec4u8[]>(size.x() * size.y());
		auto image = std::make_unique<Image4u8>(size, std::move(data), false, false);

		for (unsigned i = 0; i < size.y(); ++i)
		{
			for (unsigned j = 0; j < size.x(); ++j)
			{
				auto clr = uint8_t(i ^ j);
				image->pixel(i, j) = Vec4u8(clr, clr, clr, clr);
			}
		}
		return image;
	}

	//----------------------------------------------------------------------------------------------
	std::shared_ptr<Image4u8> Image4u8::load(std::string_view _name, bool srgb)
	{
		auto file = core::FileSystem::get()->readFile(_name.data());
		return loadFromMemory(file->buffer(), file->size(), srgb);
	}

	//----------------------------------------------------------------------------------------------
	std::shared_ptr<Image4u8> Image4u8::loadFromMemory(const void* data, size_t bufferSize, bool srgb)
	{
		if (bufferSize > 0)
		{
			// Process in memory data
			int width, height, realNumChannels;
			const auto expectedChannels = 4;
			uint8_t* imgData = stbi_load_from_memory((const uint8_t*)data, bufferSize, &width, &height, &realNumChannels, expectedChannels);

			// Create the actual image
			if (imgData)
			{
				auto dataPtr = std::unique_ptr<Vec4u8[]>(reinterpret_cast<Vec4u8*>(imgData));
				math::Vec2u size = { unsigned(width), unsigned(height) };
				return std::make_shared<Image4u8>(size, std::move(dataPtr), srgb, true);
			}
		}

		return nullptr;
	}

	//----------------------------------------------------------------------------------------------
	Image<float, 3>::Image(const Vec2u& size)
		: Image<void, 0>(
			size,
			vk::Format::eR32G32B32Sfloat,
			3,
			std::make_unique<uint8_t[]>(sizeof(Vec3f)* size.x()* size.y()))
	{
	}

	//----------------------------------------------------------------------------------------------
	Image3f::Image(const Vec2u& size, std::unique_ptr<math::Vec3f[]>&& data, bool stbiAlloc)
		: Image<void, 0>(
			size,
			vk::Format::eR32G32B32Sfloat,
			3,
			std::unique_ptr<uint8_t[]>((uint8_t*)data.release()))
		, m_isStbiAllocated(stbiAlloc)
	{
	}

	//----------------------------------------------------------------------------------------------
	Image3f::~Image()
	{
		if (m_isStbiAllocated)
		{
			stbi_image_free(m_data.release());
		}
	}

	//----------------------------------------------------------------------------------------------
	std::shared_ptr<Image3f> Image3f::load(std::string_view _name)
	{
		auto file = core::FileSystem::get()->readFile(_name.data());
		return loadFromMemory(file->buffer(), file->size());
	}

	//----------------------------------------------------------------------------------------------
	std::shared_ptr<Image3f> Image3f::loadFromMemory(const void* data, size_t bufferSize)
	{
		if (bufferSize > 0)
		{
			// Process in memory data
			int width, height, realNumChannels;
			const auto expectedChannels = 3;
			float* imgData = stbi_loadf_from_memory((const uint8_t*)data, bufferSize, &width, &height, &realNumChannels, expectedChannels);

			// Create the actual image
			if (imgData)
			{
				auto dataPtr = std::unique_ptr<Vec3f[]>(reinterpret_cast<Vec3f*>(imgData));
				math::Vec2u size = { unsigned(width), unsigned(height) };
				return std::make_shared<Image3f>(size, std::move(dataPtr), true);
			}
		}

		return nullptr;
	}

	//----------------------------------------------------------------------------------------------
	Image<float, 4>::Image(const Vec2u& size)
		: Image<void, 0>(
			size,
			vk::Format::eR32G32B32A32Sfloat,
			4,
			std::make_unique<uint8_t[]>(sizeof(Vec4f)* size.x()* size.y()))
	{
	}

	//----------------------------------------------------------------------------------------------
	Image4f::Image(const Vec2u& size, std::unique_ptr<math::Vec4f[]>&& data, bool stbiAlloc)
		: Image<void, 0>(
			size,
			vk::Format::eR32G32B32A32Sfloat,
			4,
			std::unique_ptr<uint8_t[]>((uint8_t*)data.release()))
		, m_isStbiAllocated(stbiAlloc)
	{
	}

	//----------------------------------------------------------------------------------------------
	Image4f::~Image()
	{
		if (m_isStbiAllocated)
		{
			stbi_image_free(m_data.release());
		}
	}

	//----------------------------------------------------------------------------------------------
	std::shared_ptr<Image4f> Image4f::load(std::string_view _name)
	{
		auto file = core::FileSystem::get()->readFile(_name.data());
		return loadFromMemory(file->buffer(), file->size());
	}

	//----------------------------------------------------------------------------------------------
	std::shared_ptr<Image4f> Image4f::loadFromMemory(const void* data, size_t bufferSize)
	{
		if (bufferSize > 0)
		{
			// Process in memory data
			int width, height, realNumChannels;
			const auto expectedChannels = 4;
			float* imgData = stbi_loadf_from_memory((const uint8_t*)data, bufferSize, &width, &height, &realNumChannels, expectedChannels);

			// Create the actual image
			if (imgData)
			{
				auto dataPtr = std::unique_ptr<Vec4f[]>(reinterpret_cast<Vec4f*>(imgData));
				math::Vec2u size = { unsigned(width), unsigned(height) };
				return std::make_shared<Image4f>(size, std::move(dataPtr), true);
			}
		}

		return nullptr;
	}

	//----------------------------------------------------------------------------------------------
	void saveHDR(const Image3f& img, const std::string& fileName)
	{
		if (core::getFileExtension(fileName) != "hdr")
		{
			std::cout << "Only .hdr is supported for hdr output images\n";
		}

		assert(img.format() == vk::Format::eR32G32B32Sfloat);
		auto src = reinterpret_cast<const float*>(img.data());
		stbi_write_hdr(fileName.c_str(), img.width(), img.height(), 3, src);
	}

	//----------------------------------------------------------------------------------------------
	void save2sRGB(const Image3f& img, const std::string& fileName)
	{
		if (core::getFileExtension(fileName) != "png")
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

	//----------------------------------------------------------------------------------------------
	void saveLinear(const Image3f& img, const std::string& fileName)
	{
		if (core::getFileExtension(fileName) != "png")
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