//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On 2013/Aug/22
//----------------------------------------------------------------------------------------------------------------------
// Image
#ifndef _REV_VIDEO_TYPES_IMAGE_IMAGE_H_
#define _REV_VIDEO_TYPES_IMAGE_IMAGE_H_

#include <cstdint>

namespace rev { namespace video {

	class Image {
	public:
		Image(uint8_t* _buffer, unsigned _width, unsigned _height);
		~Image();
		const uint8_t* buffer() const { return mBuffer; }
		unsigned width() const { return mWidth; }
		unsigned height() const { return mHeight; }

		static Image* loadImage(const char* _file, bool _watch = true);

	private:
		uint8_t* mBuffer;
		unsigned mWidth, mHeight;
	};

}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_TYPES_IMAGE_IMAGE_H_