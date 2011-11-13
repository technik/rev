////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernndez-Agera Tortosa (a.k.a. Technik)
// Created on November 13th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Png utils for android

#ifdef ANDROID
#include <libs/android/libpng/png.h>

#include "revCore/codeTools/assert/assert.h"
#include "revCore/codeTools/log/log.h"

namespace rev { namespace video
{
	struct pngIOBuffer
	{
		char * mBuffer;
		unsigned mCounter;
	};

	// Function prototypes
	void readFromPngBuffer(png_structp _pngPtr, png_bytep _data, png_size_t _length);

	// PNG Error handlers
	void user_error_fn(png_structp /*png_ptr*/, png_const_charp /*error_msg*/)
	{
		// Throw an assert in case of fault
		codeTools::revAssert(false);
	}

	void user_warning_fn(png_structp /*png_ptr*/, png_const_charp /*warning_msg*/)
	{
		// do nothing
	}

	//------------------------------------------------------------------------------------------------------------------
	void * imageFromPngBuffer(char * _buffer, int& _width, int& _height)
	{
		LOG_ANDROID("---imageFromPngBuffer ---");
		codeTools::revAssert(0 != _buffer);
		LOG_ANDROID("Non-null buffer");
		// Create an IO png buffer
		pngIOBuffer ioBuffer = {_buffer, 0 };
		// Set png error handling function
		// TODO

		// Check if the _buffer corresponds to a png file
		if (0 != png_sig_cmp(reinterpret_cast<png_byte*>(_buffer), 0, 8))
		{
			// Not a png file
			codeTools::revAssert(false);
		}

		LOG_ANDROID("Valid file signature");

		// Allocate the necessary png structs
		png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
		if (!png_ptr) codeTools::revAssert(false);

		LOG_ANDROID("png_structp created successfuly");

		png_infop info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr)
		{
			png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
			codeTools::revAssert(false);
		}

		LOG_ANDROID("png_infop created successfuly");

		png_infop end_info = png_create_info_struct(png_ptr);
		if (!end_info)
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
			codeTools::revAssert(false);
		}

		LOG_ANDROID("png_infop created successfuly");

		// Set proper IO method
		png_set_read_fn(png_ptr, &ioBuffer, readFromPngBuffer);

		LOG_ANDROID("png set read function");

		png_read_info(png_ptr, info_ptr);

		LOG_ANDROID("png read info successfuly");

		unsigned width = png_get_image_width(png_ptr, info_ptr);
		unsigned height = png_get_image_height(png_ptr, info_ptr);
		unsigned bytesPerRow = png_get_rowbytes(png_ptr,info_ptr);

		LOG_ANDROID("png read image metrics successfuly");

		png_bytep * row_pointers = new png_bytep[height];
		for (unsigned i=0; i<height; ++i)
		{
			row_pointers[i] = new png_byte[bytesPerRow];
		}

		LOG_ANDROID("png read row pointers successfuly");

		png_read_image(png_ptr, row_pointers);

		LOG_ANDROID("png read image successfuly");

		// allocate image buffer
		unsigned char * imgBuffer = new unsigned char[4 * width * height];
		// Copy the image
		if(4*width == bytesPerRow)
		{
			for(unsigned i = 0; i < height; ++i)
			{
				// Copy current row
				for(unsigned j = 0; j < bytesPerRow; ++j)
				{
					imgBuffer[i * bytesPerRow + j] = row_pointers[i][j];
				}
				// Free the row
				delete[] row_pointers[i];
			}
		}else if(3*width == bytesPerRow)
		{
			for(unsigned j = 0; j < height; ++j)
			{
				for(unsigned i = 0; i < width; ++i)
				{
					imgBuffer[j*4*width+4*i+0] = row_pointers[j][3*i+0];
					imgBuffer[j*4*width+4*i+1] = row_pointers[j][3*i+1];
					imgBuffer[j*4*width+4*i+2] = row_pointers[j][3*i+2];
					imgBuffer[j*4*width+4*i+3] = 255;
				}
			}
		}
		else
		{
			codeTools::revAssert(false);
		}
		// Delete rows array
		delete[] row_pointers;

		// Assign image metrics
		_width = width;
		_height = height;

		LOG_ANDROID("png successfuly loaded");

		return imgBuffer;
	}

	//------------------------------------------------------------------------------------------------------------------
	void readFromPngBuffer(png_structp _pngPtr, png_bytep _data, png_size_t _length)
	{
		pngIOBuffer * ioBuffer = reinterpret_cast<pngIOBuffer*>(png_get_io_ptr(_pngPtr));
		for(unsigned i = 0; i < _length; ++i)
		{
			(reinterpret_cast<char*>(_data))[i] = ioBuffer->mBuffer[ioBuffer->mCounter + i];
		}
		ioBuffer->mCounter += _length;
	}

}	// namespace video
}	// namespace rev

#endif // ANDROID
