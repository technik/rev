//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On 2013/Aug/22
//----------------------------------------------------------------------------------------------------------------------
// Image

#include "image.h"
#ifdef _WIN32
#include <libs/windows/freeImage.h>
#endif // _WIN32
#include <revPlatform/fileSystem/fileSystem.h>
#include <revCore/codeTools/assert/assert.h>

using namespace rev::platform;

namespace rev { namespace video {

	//------------------------------------------------------------------------------------------------------------------
	Image::Image(uint8_t* _buffer, unsigned _width, unsigned _height)
		:mBuffer(_buffer)
		,mWidth(_width)
		,mHeight(_height)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	Image::~Image()
	{
		if(nullptr != mBuffer)
			delete[] mBuffer;
	}

	//------------------------------------------------------------------------------------------------------------------
	Image* Image::loadImage(const char* _file, bool _watch)
	{
#ifdef _WIN32
		// ---- try to load the texture from a file ----
		// -- Check file extension --
		// Try to read the image format from the image's signature
		FREE_IMAGE_FORMAT fIFormat = FreeImage_GetFileType(_file, 0);
		if(fIFormat == FIF_UNKNOWN) // If failed, try to read it from the file extension
			fIFormat = FreeImage_GetFIFFromFilename(_file);
		// If we still don't have a valid extension
		if(fIFormat == FIF_UNKNOWN) {
			revLog() << "Couldn't load image: " << _file << "\n"; // Couldn't load the bitmap
			return nullptr;
		}

		// -- Actual loading --
		FIBITMAP * pFIBitmap = FreeImage_Load(fIFormat, _file); // Load the file as a bitmap
		if(nullptr == pFIBitmap) {
			revLog() << "Couldn't load image: " << _file << "\n"; // Couldn't load the bitmap
			return nullptr;
		}

		unsigned width = FreeImage_GetWidth(pFIBitmap);
		unsigned height = FreeImage_GetHeight(pFIBitmap); // Get the image size
		unsigned char * buffer = FreeImage_GetBits(pFIBitmap); // Get the pixels from the bitmap
		unsigned nPixels = width * height;
		unsigned char * pixels = new unsigned char[4*nPixels];
		if(FreeImage_GetBPP(pFIBitmap) == 32) // RGBA
		{
			for(unsigned i = 0; i < nPixels; i++)
			{
				pixels[4*i+0] = buffer[4*i+0];
				pixels[4*i+1] = buffer[4*i+1];
				pixels[4*i+2] = buffer[4*i+2];
				pixels[4*i+3] = buffer[4*i+3];
			}
		}
		else if(FreeImage_GetBPP(pFIBitmap) == 24) // RGB
		{
			for(unsigned i = 0; i < nPixels; i++)
			{
				pixels[4*i+0] = buffer[3*i+2];
				pixels[4*i+1] = buffer[3*i+1];
				pixels[4*i+2] = buffer[3*i+0];
				pixels[4*i+3] = 255;
			}
		}
		else if(FreeImage_GetBPP(pFIBitmap) == 16) // Intensity-Alpha
		{
			for(unsigned i = 0; i < nPixels; i++)
			{
				pixels[4*i+0] = buffer[2*i+0];
				pixels[4*i+1] = buffer[2*i+0];
				pixels[4*i+2] = buffer[2*i+0];
				pixels[4*i+3] = buffer[2*i+0];
			}
		}
		else if(FreeImage_GetBPP(pFIBitmap) == 8) // Grey image
		{
			for(unsigned i = 0; i < nPixels; i++)
			{
				pixels[4*i+0] = buffer[i];
				pixels[4*i+1] = buffer[i];
				pixels[4*i+2] = buffer[i];
				pixels[4*i+3] = 255;
			}
		}
		else
		{
			revLog() << "Unsupported image format (" << _file << ")\n";
			delete[] pixels;
			FreeImage_Unload(pFIBitmap); // Release FreeImage's copy of the data
			return nullptr;
		}
		FreeImage_Unload(pFIBitmap); // Release FreeImage's copy of the data
		// Construct a texture using the data we just loaded
		Image* image = new Image(pixels, width, height);
#else //!_WIN32
		Image* image = nullptr;
#endif //!_WIN32
		revAssert(nullptr != image);
		if(_watch) {
			FileSystem::get()->onFileChanged(_file) += [=](const char* _fileName) {
				Image* newImage = loadImage(_fileName, false);
				image->mBuffer = newImage->mBuffer;
				image->mWidth = newImage->mWidth;
				image->mHeight = newImage->mHeight;
				newImage->mBuffer = nullptr;
				delete newImage;
			};
		}
		return image;
	}

}	// namespace video
}	// namespace rev