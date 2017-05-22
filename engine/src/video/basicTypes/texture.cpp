//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Textures
#ifndef ANDROID

#include <video/basicTypes/texture.h>
#include <freeImage/FreeImage.h>
#include <video/graphics/driver/graphicsDriver.h>
#include <cjson/json.h>
#include <fstream>

using namespace cjson;

namespace rev {
	namespace video {
		
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
		Texture::Texture(const TextureInfo& _desc, const ImageBuffer* _buffers, size_t nMips)
			: mSize(_desc.size)
			, mMultiSample(false)
		{
			// Generate OpenGL texture buffer
			glGenTextures(1, &mId);
			glBindTexture(GL_TEXTURE_2D, mId);

			if(_desc.genMips)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			else
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)_desc.filter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)_desc.filter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)_desc.repeat);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)_desc.repeat);

			// Load mipmaps
			constexpr int borderSize = 0;
			for (size_t mipLevel = 0; mipLevel < nMips; ++mipLevel) {
				const ImageBuffer& buffer = _buffers[mipLevel];
				glTexImage2D(GL_TEXTURE_2D, mipLevel, (GLint)_desc.gpuFormat,
					_desc.size.x, _desc.size.y, borderSize, (GLenum)buffer.fmt, GL_UNSIGNED_BYTE, buffer.data);
			}
			if (_desc.genMips)
				glGenerateMipmap(GL_TEXTURE_2D);
		}

		//--------------------------------------------------------------------------------------------------------------
		Texture* Texture::loadFromFile(const std::string& _fileName) {
			Json textureData;
			textureData.parse(std::ifstream(_fileName));
			// Load format and texture info
			TextureInfo desc;
			desc.gpuFormat = colorBufferFormat(textureData["channels"]);
			desc.genMips = (bool)textureData["genMips"];
			desc.repeat = (bool)textureData["repeat"];
			desc.filter = filterMode(textureData["filter"]);
			const Json& mips = textureData["mips"];
			// Load buffers
			ImageBuffer mipBuffers[16];
			for (size_t i = 0; i < mips.size(); ++i) {
				if (!loadBuffer((std::string)mips(i), mipBuffers[i])) {
					// clear previous mipmaps
					for (size_t j = 0; j < i; ++j) {
						delete[] mipBuffers[j].data;
					}
					return nullptr;
				}
			}
			desc.size = mipBuffers[0].size;
			return new Texture(desc, mipBuffers, mips.size());
		}

		//--------------------------------------------------------------------------------------------------------------
		Texture::FilterMode Texture::filterMode(const std::string& _s) {
			FilterMode mode = FilterMode::linear;
			if (_s == "nearest")
				mode = FilterMode::nearest;
			return mode;
		}

		//--------------------------------------------------------------------------------------------------------------
		Texture::InternalFormat Texture::colorBufferFormat(const std::string& _s) {
			InternalFormat fmt = InternalFormat::rgba;
			if(_s == "rgb")
				fmt = InternalFormat::rgb;
			else if(_s == "rg")
				fmt = InternalFormat::rg;
			else if (_s == "r")
				fmt = InternalFormat::r;
			return fmt;
		}

		//--------------------------------------------------------------------------------------------------------------
		bool Texture::loadBuffer(const std::string& _fileName, ImageBuffer& _dst) {
			auto fIFormat = FreeImage_GetFIFFromFilename(_fileName.c_str());
			int flags = 0;
			if (fIFormat == FIF_JPEG)
				flags = JPEG_ACCURATE;
			FIBITMAP * bitmap = FreeImage_Load(fIFormat, _fileName.c_str(), flags);
			if (!bitmap)
				return false;
			auto imgType = FreeImage_GetImageType(bitmap);
			auto colorType = FreeImage_GetColorType(bitmap);
			if (imgType != FIT_BITMAP || colorType != FIC_RGB) {
				FreeImage_Unload(bitmap);
				return false;
			}

			_dst.size.x = FreeImage_GetWidth(bitmap);
			_dst.size.y = FreeImage_GetHeight(bitmap);

			auto bitsPerPixel = FreeImage_GetBPP(bitmap);
			switch (bitsPerPixel)
			{
			case 32:
				_dst.fmt = SourceFormat::rgba;
				break;
			case 24:
				_dst.fmt = SourceFormat::rgb;
				break;
			case 16:
				_dst.fmt = SourceFormat::rg;
				break;
			case 8:
				_dst.fmt = SourceFormat::r;
				break;
			default:
				// Unsupported image format
				FreeImage_Unload(bitmap);
				return false;
			}
			// Copy data
			size_t nPixels = _dst.size.x*_dst.size.y;
			size_t buffSize = nPixels * bitsPerPixel / 8;
			_dst.data = new uint8_t[buffSize];
			memcpy(_dst.data, FreeImage_GetBits(bitmap), buffSize);
			FreeImage_Unload(bitmap); // Release FreeImage's copy of the data
			// Correct channels order if necessary
			if(bitsPerPixel  == 24) {
				// Swap b and r channels
				for (unsigned i = 0; i < nPixels; ++i)
				{
					uint8_t r = _dst.data[3 * i + 2];
					_dst.data[3 * i + 2] = _dst.data[3 * i];
					_dst.data[3 * i] = r;
				}
			}
			return true;
		}
	}
}

#endif // !ANDROID