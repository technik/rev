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
		Texture::Texture(const math::Vec2u& _size, InternalFormat _targetFormat, SourceFormat _srcFormat, uint8_t* _data)
			: mSize(_size)
		{
			glGenTextures(1, &mId);
			glBindTexture(GL_TEXTURE_2D, mId);
			glTexImage2D(GL_TEXTURE_2D, 0, (GLint)_targetFormat, _size.x, _size.y, 0, (GLenum)_srcFormat, GL_UNSIGNED_BYTE, _data);
			// Default settings
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		}
		
		//--------------------------------------------------------------------------------------------------------------
		Texture::Texture(const math::Vec2u& _size, InternalFormat _targetFormat, bool _multiSample)
			: mSize(_size)
			, mMultiSample(_multiSample)
		{
			GLenum target = _multiSample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
			glGenTextures(1, &mId);
			glBindTexture(target, mId);
			
			if (_multiSample){
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, (GLint)_targetFormat, _size.x, _size.y, true);
				glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				//glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
				//glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
			}
			else {
				GLenum srcFormat = (GLenum)_targetFormat;
				if (_targetFormat == InternalFormat::rgb)
					srcFormat = GL_RGB;
				else if (_targetFormat == InternalFormat::rgba)
					srcFormat = GL_RGBA;
				if (_targetFormat == InternalFormat::rgb)
					srcFormat = GL_RGB;
				else if (_targetFormat == InternalFormat::rg)
					srcFormat = GL_RG;
				else if (_targetFormat == InternalFormat::rg32f)
					srcFormat = GL_RG;
				else if (_targetFormat == InternalFormat::rg16f)
					srcFormat = GL_RG;
				else if (_targetFormat == InternalFormat::r)
					srcFormat = GL_RED;
				if(_targetFormat == InternalFormat::depth) {
					glTexImage2D(GL_TEXTURE_2D, 0, (GLint)_targetFormat, _size.x, _size.y, 0, (GLint)_targetFormat, GL_FLOAT, nullptr);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					// Remove artefact on the edges of the shadowmap
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				}
				else {
					glTexImage2D(GL_TEXTURE_2D, 0, (GLint)_targetFormat, _size.x, _size.y, 0, srcFormat, GL_UNSIGNED_BYTE, nullptr);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				}
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
			int flags = 0;
			if(fIFormat == FIF_JPEG)
				flags = JPEG_ACCURATE;
			FIBITMAP * bitmap = FreeImage_Load(fIFormat, _fileName.c_str(), flags);
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
			Texture* tex = nullptr;
			switch (FreeImage_GetBPP(bitmap))
			{
			case 32:
				tex = new Texture(size, InternalFormat::rgba, SourceFormat::rgba, buffer);
				break;
			case 24: {
				unsigned nPixels = size.x*size.y;
				for (unsigned i = 0; i < nPixels; ++i)
				{
					uint8_t r = buffer[3*i+2];
					buffer[3 * i + 2] = buffer[3 * i];
					buffer[3 * i] = r;
				}
				tex = new Texture(size, InternalFormat::rgb, SourceFormat::rgb, buffer);
				break;
			}
			case 16:
				tex = new Texture(size, InternalFormat::rg, SourceFormat::rg, buffer);
				break;
			case 8:
				tex = new Texture(size, InternalFormat::r, SourceFormat::r, buffer);
				break;
			}
			FreeImage_Unload(bitmap); // Release FreeImage's copy of the data
										 // Construct a texture using the data we just loaded
			
			return tex;
		}

		//--------------------------------------------------------------------------------------------------------------
		void Texture::enableMipMaps() {
			glBindTexture(GL_TEXTURE_2D, mId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}

		//--------------------------------------------------------------------------------------------------------------
		void Texture::generateMipMaps() {
			glBindTexture(GL_TEXTURE_2D, mId);
			glGenerateMipmap(GL_TEXTURE_2D);  //Generate mipmaps now!!!
		}
	}
}