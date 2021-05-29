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

#include <cstdint>
#include <string_view>
#include <memory>
#include <math/algebra/vector.h>

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>
#include <graphics/types.h>

namespace rev::gfx
{
	// Type erased base class of all images
	template<class T=void, size_t N=0>
	class Image
	{
	public:
		static vk::Format GetPixelFormat(bool hdr, unsigned numChannnels, bool srgb);

		// Accessors
		vk::Format	format()	const { return m_format; }
		auto&		size()		const { return m_size; }
		auto		byteSize()	const { return m_size.x() * m_size.y() * m_byteStride; }
		uint32_t	area()		const { return m_size.x() * m_size.y(); }
		uint32_t	width()		const { return m_size.x(); }
		uint32_t	height()	const { return m_size.y(); }

		void* data() const { return m_data.get(); }

		// Note: nChannels = 0 sets automatic number of channels
		static std::shared_ptr<Image> load(std::string_view _name, unsigned nChannels, bool hdr, bool srgb);
		static std::shared_ptr<Image> loadFromMemory(const void* data, size_t size, unsigned nChannels, bool hdr, bool srgb);

	protected:
		Image(const math::Vec2u& size, vk::Format format, uint32_t byteStride, std::unique_ptr<uint8_t[]>&& data);
		~Image() = default;

		template<typename T2 = void>
		auto dataAs() { return reinterpret_cast<T2*>(m_data.get()); }
		template<typename T2 = void>
		auto dataAs() const { return reinterpret_cast<const T2*>(m_data.get()); }

		template<typename T>
		auto& pixelAs(const math::Vec2u& pos) { return dataAs<T>()[indexFromPos(pos)]; }
		template<typename T>
		auto& pixelAs(const math::Vec2u& pos) const { return dataAs<T>()[indexFromPos(pos)]; }

		__forceinline size_t indexFromPos(const math::Vec2u& pos) const
		{
			return pos.x() + pos.y() * m_size.x();
		}
		__forceinline size_t indexFromPos(unsigned x, unsigned y) const
		{
			return x + y * m_size.x();
		}

	private:
		Image(const Image&) = delete;
		Image& operator=(const Image&) = delete;

	protected:
		std::unique_ptr<uint8_t[]> m_data;
		math::Vec2u m_size;
		vk::Format m_format;
		uint32_t m_byteStride;
	};

	template<>
	class Image<uint8_t,3> : public Image<void,0>
	{
	public:
		Image(const math::Vec2u& size, bool srgb);
		Image(const math::Vec2u& size, std::unique_ptr<math::Vec3u8[]>&& data, bool srgb, bool stbiAlloc = false);
		~Image();

		auto		data() { return dataAs<math::Vec3u8>(); }
		auto		data() const { return dataAs<math::Vec3u8>(); }
		auto& pixel(const math::Vec2u& pos) { return data()[indexFromPos(pos)]; }
		auto& pixel(const math::Vec2u& pos) const { return data()[indexFromPos(pos)]; }

		auto& pixel(unsigned x, unsigned y) { return data()[indexFromPos(x, y)]; }
		auto& pixel(unsigned x, unsigned y) const { return data()[indexFromPos(x, y)]; }

		// XOR textures are always 8-bits per channel
		static std::shared_ptr<Image<uint8_t, 3>> proceduralXOR(const math::Vec2u& size);

		static std::shared_ptr<Image<uint8_t, 3>> load(std::string_view _name, bool srgb);
		static std::shared_ptr<Image<uint8_t, 3>> loadFromMemory(const void* data, size_t size, bool srgb);

	private:
		bool m_isStbiAllocated = false;
	};

	template<>
	class Image<uint8_t, 4> : public Image<void, 0>
	{
	public:
		Image(const math::Vec2u& size, bool srgb);
		Image(const math::Vec2u& size, std::unique_ptr<math::Vec4u8[]>&& data, bool srgb, bool stbiAlloc = false);
		~Image();

		auto		data() { return dataAs<math::Vec4u8>(); }
		auto		data() const { return dataAs<math::Vec4u8>(); }
		auto& pixel(const math::Vec2u& pos) { return data()[indexFromPos(pos)]; }
		auto& pixel(const math::Vec2u& pos) const { return data()[indexFromPos(pos)]; }

		auto& pixel(unsigned x, unsigned y) { return data()[indexFromPos(x, y)]; }
		auto& pixel(unsigned x, unsigned y) const { return data()[indexFromPos(x, y)]; }

		// XOR textures are always 8-bits per channel
		static std::shared_ptr<Image<uint8_t, 4>> proceduralXOR(const math::Vec2u& size);

		static std::shared_ptr<Image<uint8_t, 4>> load(std::string_view _name, bool srgb);
		static std::shared_ptr<Image<uint8_t, 4>> loadFromMemory(const void* data, size_t size, bool srgb);

	private:
		bool m_isStbiAllocated = false;
	};

	template<>
	class Image<float, 3> : public Image<void, 0>
	{
	public:
		Image(const math::Vec2u& size);
		Image(const math::Vec2u& size, std::unique_ptr<math::Vec3f[]>&& data, bool stbiAlloc = false);
		~Image();

		auto		data() { return dataAs<math::Vec3f>(); }
		auto		data() const { return dataAs<math::Vec3f>(); }
		auto& pixel(const math::Vec2u& pos) { return data()[indexFromPos(pos)]; }
		auto& pixel(const math::Vec2u& pos) const { return data()[indexFromPos(pos)]; }

		auto& pixel(unsigned x, unsigned y) { return data()[indexFromPos(x, y)]; }
		auto& pixel(unsigned x, unsigned y) const { return data()[indexFromPos(x, y)]; }

		static std::shared_ptr<Image<float, 3>> load(std::string_view _name);
		static std::shared_ptr<Image<float, 3>> loadFromMemory(const void* data, size_t size);

	private:
		bool m_isStbiAllocated = false;
	};

	template<>
	class Image<float, 4> : public Image<void, 0>
	{
	public:
		Image(const math::Vec2u& size);
		Image(const math::Vec2u& size, std::unique_ptr<math::Vec4f[]>&& data, bool stbiAlloc = false);
		~Image();

		auto		data() { return dataAs<math::Vec4f>(); }
		auto		data() const { return dataAs<math::Vec4f>(); }
		auto& pixel(const math::Vec2u& pos) { return data()[indexFromPos(pos)]; }
		auto& pixel(const math::Vec2u& pos) const { return data()[indexFromPos(pos)]; }

		auto& pixel(unsigned x, unsigned y) { return data()[indexFromPos(x,y)]; }
		auto& pixel(unsigned x, unsigned y) const { return data()[indexFromPos(x, y)]; }

		static std::shared_ptr<Image<float, 4>> load(std::string_view _name);
		static std::shared_ptr<Image<float, 4>> loadFromMemory(const void* data, size_t size);

	private:
		bool m_isStbiAllocated = false;
	};

	using Image3u8 = Image<uint8_t, 3>;
	using Image4u8 = Image<uint8_t, 4>;
	using Image3f = Image<float, 3>;
	using Image4f = Image<float, 4>;

	void saveHDR(const Image<float,3>& img, const std::string& fileName);

	void save2sRGB(const Image<float,3>& img, const std::string& fileName);

	void saveLinear(const Image<float,3>& img, const std::string& fileName);

} // namespace rev::gfx
