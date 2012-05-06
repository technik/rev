////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 30th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Textures

#include "texture.h"

#ifdef _WIN32
#include <windows.h>
#include <gl/GL.h>
#include "libs/windows/freeImage.h"
#endif // _WIn32

#ifdef ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "android/androidpngutils.h"
#include "revCore/file/file.h"

#define GL_VERBOSE
#ifdef GL_VERBOSE
#define GL_LOG( ... ) LOG_ANDROID( __VA_ARGS__ )
#else
#define GL_LOG( ... )
#endif

#define ASSERT_GL( ... ) \
{int errorCode = glGetError();\
if(GL_NO_ERROR != errorCode) \
{\
	switch(errorCode)\
	{\
		case GL_INVALID_OPERATION:\
		{\
			LOG_ANDROID("GL_INVALID_OPERATION");\
			break;\
		}\
		case GL_INVALID_VALUE:\
		{\
			LOG_ANDROID("GL_INVALID_VALUE");\
			break;\
		}\
		case GL_INVALID_ENUM:\
		{\
			LOG_ANDROID("GL_INVALID_ENUM");\
			break;\
		}\
		default:\
		{\
			LOG_ANDROID("GL_OTHER");\
			break;\
		}\
	}\
	LOG_ANDROID( __VA_ARGS__ );\
	codeTools::revAssert(false);\
}}

#endif // ANDROID

#include <revCore/codeTools/assert/assert.h>
#include <revCore/codeTools/log/log.h>
#include <revVideo/video.h>
#include <revVideo/videoDriver/videoDriver.h>

namespace rev {
	//------------------------------------------------------------------------------------------------------------------
	// Static data definition
	video::CTexture::TManager * video::CTexture::sManager = 0;
namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CTexture::CTexture(void * _buffer, int _width, int _height)
		:mBuffer(_buffer)
		,mWidth(_width)
		,mHeight(_height)
	{
		// Copy data into texture
		mBuffer = _buffer;
		mWidth = _width;
		mHeight = _height;

		mId = SVideo::getDriver()->registerTexture(mBuffer, mWidth, mHeight);
	}

	//------------------------------------------------------------------------------------------------------------------
	CTexture::CTexture(const char* _name)
	{
#ifdef ANDROID
		char * fileBuffer = bufferFromFile(_name);

		mBuffer = imageFromPngBuffer(fileBuffer, mWidth, mHeight);

		delete [] fileBuffer;

#endif // ANDROID
#ifdef _WIN32
		// ---- try to load the texture from a file ----
		// -- Check file extension --
		// Try to read the image format from the image's signature
		FREE_IMAGE_FORMAT fIFormat = FreeImage_GetFileType(_name, 0);
		if(fIFormat == FIF_UNKNOWN) // If failed, try to read it fro the file extension
			fIFormat = FreeImage_GetFIFFromFilename(_name);
		// If we still don't have a valid extension
		if(fIFormat == FIF_UNKNOWN)
			revAssert(0); // Couldn't get a valid file extension

		// -- Actual loading --
		FIBITMAP * pFIBitmap = FreeImage_Load(fIFormat, _name); // Load the file as a bitmap
		revAssert(0 != pFIBitmap, "Couldn't load the bitmap"); // Couldn't load the bitmap

		mWidth = FreeImage_GetWidth(pFIBitmap);
		mHeight = FreeImage_GetHeight(pFIBitmap); // Get the image size
		unsigned char * buffer = FreeImage_GetBits(pFIBitmap); // Get the pixels from the bitmap
		unsigned nPixels = mWidth * mHeight;
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
		else // RGB
		{
			for(unsigned i = 0; i < nPixels; i++)
			{
				pixels[4*i+0] = buffer[3*i+2];
				pixels[4*i+1] = buffer[3*i+1];
				pixels[4*i+2] = buffer[3*i+0];
				pixels[4*i+3] = 255;
			}
		}
		FreeImage_Unload(pFIBitmap); // Release FreeImage's copy of the data
		// Construct a texture using the data we just loaded
		mBuffer = pixels;
#endif // _WIN32
		mId = SVideo::getDriver()->registerTexture(mBuffer, mWidth, mHeight);
	}

	//------------------------------------------------------------------------------------------------------------------
	CTexture::~CTexture()
	{
		// TODO: Solve GL Leak
#ifdef ANDROID
		delete reinterpret_cast<unsigned char *>(mBuffer);
#else // !ANDROID
		delete mBuffer;
#endif // !ANDROID
		glDeleteTextures(1, &mId);
	}

}	// namespace video
}	// namespace rev
