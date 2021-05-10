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

namespace rev::gfx {

	// An image must always have a pixel format, even if it's empty.
	// This format, however, is mutable, and can be changed when assigning new content to the image
	class Image
	{
	public:

		enum class ChannelFormat : std::uint8_t
		{
			Byte,
			Float32
		};

		struct PixelFormat
		{
			ChannelFormat channel;
			std::uint8_t numChannels;

			size_t pixelSize() const {
				return numChannels * ((channel == ChannelFormat::Byte)?1:4);
			}

			bool operator==(const PixelFormat& b) const
			{
				return channel == b.channel && numChannels == b.numChannels;
			}
		};

		Image(PixelFormat, const math::Vec2u& size = math::Vec2u::zero());
		Image(const Image&) = delete;
		Image(Image&&);

		// Constructors from specific color formats
		Image(const math::Vec2u& size, math::Vec3u8* data);
		Image(const math::Vec2u& size, math::Vec4u8* data);
		Image(const math::Vec2u& size, math::Vec3f* data);
		Image(const math::Vec2u& size, math::Vec4f* data);

		~Image();

		Image& operator=(const Image&) = delete;
		Image& operator=(Image&&);

		// Modifiers
		void		clear(); ///< Clears the size, but doesn't free the storage buffer, so capacity remains intact
		void		erase(); ///< Erases all data, and restores both size and capacity to 0
		void		resize(const math::Vec2u& size); ///< Resizing invalidates the original contents
		void		setPixelFormat(PixelFormat);

		// Accessors
		PixelFormat format()	const	{ return mFormat; }
		auto&		size()		const	{ return mSize; }
		size_t		area()		const	{ return size().x() * size().y(); }
		template<typename T = void*>
		auto		data()				{ return reinterpret_cast<T*>(mData); }
		template<typename T = const void*>
		auto		data()		const	{ return reinterpret_cast<const T*>(mData); }
		template<typename T>
		auto&		pixel(const math::Vec2u& pos)				{ return data<T>()[indexFromPos(pos)]; }
		template<typename T>
		auto&		pixel(const math::Vec2u& pos) const			{ return data<T>()[indexFromPos(pos)]; }

		// XOR textures are always 8-bits per channel
		static Image proceduralXOR(const math::Vec2u& size, size_t nChannels);

		// Note: nChannels = 0 sets automatic number of channels
		static std::shared_ptr<Image> load(std::string_view _name, unsigned nChannels);
		static std::shared_ptr<Image> loadFromMemory(const void* data, size_t size, unsigned nChannels);

	private:
		// Base constructor from size and data
		Image(PixelFormat, const math::Vec2u& size, void* rawData);
		static void* allocatePixelData(PixelFormat pxlFormat, size_t numPixels);
		size_t indexFromPos(const math::Vec2u&) const;
		size_t rawDataSize() const; ///< Size in number of bytes of the allocated storage buffer

	private:
		math::Vec2u mSize;
		size_t		mCapacity;
		PixelFormat mFormat;
		void*		mData = nullptr;
	};

} // namespace rev::gfx
