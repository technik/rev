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

#include "revCore/codeTools/assert/assert.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	// Static data
	CTexture::managerT * CTexture::sManager = 0;

	//------------------------------------------------------------------------------------------------------------------
	CTexture::CTexture(const string& _name)
	{
#ifdef _WIN32
		// ---- try to load the texture from a file ----
		// -- Check file extension --
		// Try to read the image format from the image's signature
		FREE_IMAGE_FORMAT fIFormat = FreeImage_GetFileType(_name.c_str(), 0);
		if(fIFormat == FIF_UNKNOWN) // If failed, try to read it fro the file extension
			fIFormat = FreeImage_GetFIFFromFilename(_name.c_str());
		// If we still don't have a valid extension
		if(fIFormat == FIF_UNKNOWN)
			codeTools::revAssert(0); // Couldn't get a valid file extension

		// -- Actual loading --
		FIBITMAP * pFIBitmap = FreeImage_Load(fIFormat, _name.c_str()); // Load the file as a bitmap
		if(!pFIBitmap)
			codeTools::revAssert(false); // Couldn't load the bitmap

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
		// Get an openGL texture ID for this texture
		glGenTextures(1, &mId);
		// Load the image to the graphic card
		glBindTexture(GL_TEXTURE_2D, mId);
		// Basic texture configuration
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//store the texture data for OpenGL use
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight,
			0, GL_RGBA, GL_UNSIGNED_BYTE, mBuffer);
#endif // _WIN32
	}

	//------------------------------------------------------------------------------------------------------------------
	CTexture::~CTexture()
	{
		delete mBuffer;
		// TODO: Solve GL Leak
	}

}	// namespace video
}	// namespace rev