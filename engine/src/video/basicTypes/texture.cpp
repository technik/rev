//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Textures

#include <video/basicTypes/texture.h>
#include <freeImage/FreeImage.h>
#include <video/graphics/driver/graphicsDriver.h>

namespace rev {
	namespace video {

		//--------------------------------------------------------------------------------------------------------------
		Texture::Texture(const math::Vec2u& _size, Texture::EImageFormat _if, Texture::EByteFormat _bf, void* _data, bool _multiSample)
			: size(_size)
			, data((uint8_t*)_data)
			, imgFormat(_if)
			, byteFormat(_bf)
			, mMultiSample(_multiSample)
		{
			GLenum target = _multiSample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
			glGenTextures(1, &mId);
			glBindTexture(target, mId);
			GLint format = enumToGl(_if);
			GLint byteFormat = enumToGl(_bf);
			if (_multiSample)
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, format, _size.x, _size.y, true);
			else {
				glTexImage2D(GL_TEXTURE_2D, 0, format, _size.x, _size.y, 0, format, enumToGl(_bf), _data);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			}
		}

		//--------------------------------------------------------------------------------------------------------------
		Texture::~Texture()
		{
			glGenTextures(1, &mId);
			glDeleteTextures(1, &mId);
		}

		//--------------------------------------------------------------------------------------------------------------
		Texture* Texture::load(const std::string& _fileName, GraphicsDriver* _driver){
			auto fIFormat = FreeImage_GetFIFFromFilename(_fileName.c_str());
			FIBITMAP * bitmap = FreeImage_Load(fIFormat, _fileName.c_str(), JPEG_ACCURATE);
			if(!bitmap)
				return nullptr;
			auto imgType = FreeImage_GetImageType(bitmap);
			auto colorType = FreeImage_GetColorType(bitmap);
			if(imgType != FIT_BITMAP || colorType != FIC_RGB) {
				FreeImage_Unload(bitmap);
				return nullptr;
			}
			math::Vec2u size;
			size.x = FreeImage_GetWidth(bitmap);
			size.y = FreeImage_GetHeight(bitmap);
			uint8_t * buffer = FreeImage_GetBits(bitmap); // Get the pixels from the bitmap
			size_t nPixels = size.x * size.y;
			unsigned char * pixels = new unsigned char[4 * nPixels];
			if (FreeImage_GetBPP(bitmap) == 32) // RGBA
			{
				for (unsigned i = 0; i < nPixels; i++)
				{
					pixels[4 * i + 0] = buffer[4 * i + 0];
					pixels[4 * i + 1] = buffer[4 * i + 1];
					pixels[4 * i + 2] = buffer[4 * i + 2];
					pixels[4 * i + 3] = buffer[4 * i + 3];
				}
			}
			else if (FreeImage_GetBPP(bitmap) == 24) // RGB
			{
				for (unsigned i = 0; i < nPixels; i++)
				{
					pixels[4 * i + 0] = buffer[3 * i + 2];
					pixels[4 * i + 1] = buffer[3 * i + 1];
					pixels[4 * i + 2] = buffer[3 * i + 0];
					pixels[4 * i + 3] = 255;
				}
			}
			else if (FreeImage_GetBPP(bitmap) == 16) // Intensity-Alpha
			{
				for (unsigned i = 0; i < nPixels; i++)
				{
					pixels[4 * i + 0] = buffer[2 * i + 0];
					pixels[4 * i + 1] = buffer[2 * i + 0];
					pixels[4 * i + 2] = buffer[2 * i + 0];
					pixels[4 * i + 3] = buffer[2 * i + 0];
				}
			}
			else if (FreeImage_GetBPP(bitmap) == 8) // Grey image
			{
				for (unsigned i = 0; i < nPixels; i++)
				{
					pixels[4 * i + 0] = buffer[i];
					pixels[4 * i + 1] = buffer[i];
					pixels[4 * i + 2] = buffer[i];
					pixels[4 * i + 3] = 255;
				}
			}
			else
			{
				assert(false); //  "Unsupported image format"
				FreeImage_Unload(bitmap);
				return nullptr;
			}
			FreeImage_Unload(bitmap); // Release FreeImage's copy of the data
										 // Construct a texture using the data we just loaded
			
			return _driver->createTexture(size, EImageFormat::rgba, EByteFormat::eUnsignedByte, pixels);
		}

		//------------------------------------------------------------------------------------------------------------------
		GLint Texture::enumToGl(Texture::EImageFormat _format) {
			switch (_format) {
			case Texture::EImageFormat::rgb:
				return GL_RGB;
			case Texture::EImageFormat::rgba:
				return GL_RGBA;
			case Texture::EImageFormat::alpha:
				return GL_ALPHA;
			case Texture::EImageFormat::luminance:
				return GL_LUMINANCE;
			case Texture::EImageFormat::lumiAlpha:
				return GL_LUMINANCE_ALPHA;
			case Texture::EImageFormat::depth:
				return GL_DEPTH_COMPONENT;
			case Texture::EImageFormat::depthStencil:
				return GL_DEPTH_STENCIL;
			}
			return -1;
		}

		//------------------------------------------------------------------------------------------------------------------
		GLint Texture::enumToGl(Texture::EByteFormat _format) {
			switch (_format)
			{
			case Texture::EByteFormat::eUnsignedByte:
				return GL_UNSIGNED_BYTE;
			case Texture::EByteFormat::eByte:
				return GL_BYTE;
			case Texture::EByteFormat::eUnsignedShort:
				return GL_UNSIGNED_SHORT;
			case Texture::EByteFormat::eShort:
				return GL_SHORT;
			case Texture::EByteFormat::eUnsignedInt:
				return GL_UNSIGNED_INT;
			case Texture::EByteFormat::eInt:
				return GL_INT;
			case Texture::EByteFormat::eFloat:
				return GL_FLOAT;
			}
			return -1;
		}
	}
}