//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Textures

#include <video/basicTypes/texture.h>
#include <freeImage/FreeImage.h>
#include <video/graphics/driver/graphicsDriver.h>
#include <cjson/json.h>
#include <iostream>
#include <fstream>
#include <util/string_util.h>
#include <cmath>

using namespace cjson;
using namespace std;

namespace rev {
	namespace video {

		namespace { // PVR loading
			
			//---------------------------------------------------------------------------------------------------------
			struct PVRHeader{
				uint32_t	version;
				uint32_t	flags;
				uint64_t	pixelFormat;
				uint32_t	colourSpace;
				uint32_t	channelType;
				uint32_t	height;
				uint32_t	width;
				uint32_t	depth;
				uint32_t	numSurfaces;
				uint32_t	numFaces;
				uint32_t	mipMapCount;
				uint32_t	metaDataSize;

				//-------------------------------------------------------------------------------------
				bool checkHeader() const {
					if (!checkVersion()) {
						if (checkWrongEndian()) {
							cout << "Error: wrong endian loading PVR\n";
						}
						else
							cout << "Unsupported version loading PVR\n";
						return false;
					}
					uint32_t formatHigh = uint32_t(pixelFormat >> 32);
					if (formatHigh == 0) {
						cout << "Error: Compressed formats are not supported\n";
						return false;
					}
					return true;
				}

				//-------------------------------------------------------------------------------------
				GLint pixelChannels() const {
					char* format = (char*)&pixelFormat;
					if (format[7] == 'a') // Contains alpha
						return (GLint)Texture::SourceFormat::rgba;
					else
						return (GLint)Texture::SourceFormat::rgb;

				}

				//-------------------------------------------------------------------------------------
				Texture::InternalFormat gpuFormat() const {
					char* format = (char*)&pixelFormat;
					if (format[7] == 'a') {// Contains alpha
						if (format[0] == 32) {
							return Texture::InternalFormat::rgba32f;
						}
						else {
							return Texture::InternalFormat::rgba;
						}
					}
					else { // no alpha
						if (format[0] == 32) {
							return Texture::InternalFormat::rgb32f;
						}
						else {
							return Texture::InternalFormat::rgb;
						}
					}
				}

				bool floatData() const {
					return channelType == 12;
				}

				//-------------------------------------------------------------------------------------
				GLenum glTarget() const {
					if (depth > 1) {
						return GL_TEXTURE_3D;
					}
					else if (height > 1) {
						if (numFaces == 6) {
							if (numSurfaces > 1)
								return GL_TEXTURE_CUBE_MAP_ARRAY;
							else
								return GL_TEXTURE_CUBE_MAP;
						}
						else {
							assert(numFaces == 1);
							if (numSurfaces > 1)
								return GL_TEXTURE_2D_ARRAY;
							else
								return GL_TEXTURE_2D;
						}
					}
					else {
						if (numSurfaces > 1)
							return GL_TEXTURE_1D_ARRAY;
						else
							return GL_TEXTURE_1D;
					}
				}

			private:
				bool checkVersion() const {
					return 0x03525650 == version;
				};

				bool checkWrongEndian() const {
					return 0x50565203 == version;
				}
			};

			//--------------------------------------------------------------------------------------------------------------
			bool loadPVRImageToGL(istream& pvrFile, const Texture::TextureInfo& tInfo, const PVRHeader& header, GLint texId, GLenum glTarget) {
				math::Vec3u mipSize = tInfo.size;
				GLenum dataType = header.floatData() ? GL_FLOAT : GL_UNSIGNED_BYTE;
				GLint sourceFormat = header.pixelChannels();
				uint32_t bytesPerPixel = (sourceFormat == GL_RGB ? 3 : 4) * (dataType == GL_FLOAT ? 4 : 1);
				GLint internalFormat = (GLint)tInfo.gpuFormat;
				uint32_t mipSizeInBytes = mipSize.x*mipSize.y*mipSize.z * bytesPerPixel *header.numSurfaces;
				char* buffer = new char[mipSizeInBytes];
				for(size_t i = 0; i < mipSizeInBytes; ++i)
					buffer[i] = char(0x55);
				// Read one mip level at a time
				for (size_t mipLevel = 0; mipLevel < header.mipMapCount; ++mipLevel) {
					uint32_t curMipBytes = mipSize.x*mipSize.y*mipSize.z *header.numSurfaces * bytesPerPixel;
					if (glTarget == GL_TEXTURE_1D) { // Single dimension textures
						pvrFile.read(buffer, curMipBytes);
						glTexImage1D(glTarget, mipLevel, sourceFormat, mipSize.x, 0, internalFormat, dataType, buffer);
					}
					else {
						// Bidimensional textures
						if (glTarget == GL_TEXTURE_CUBE_MAP || glTarget == GL_TEXTURE_1D_ARRAY || glTarget == GL_TEXTURE_2D) {
							if (header.numFaces == 6) { // Cubemap
								for (GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X; face <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; ++face) {
									pvrFile.read(buffer, curMipBytes);
									glTexImage2D(face, mipLevel, internalFormat, mipSize.x, mipSize.y, 0, sourceFormat, dataType, buffer);
								}
							}
							else { // 2D or 1D_array
								pvrFile.read(buffer, curMipBytes);
								assert(header.numFaces == 1);
								uint32_t height = mipSize.y * header.numSurfaces;
								glTexImage2D(glTarget, mipLevel, internalFormat, mipSize.x, height, 0, sourceFormat, dataType, buffer);
							}
						}
						else { // Tridimensional textures
							assert(glTarget == GL_TEXTURE_3D || glTarget == GL_TEXTURE_2D_ARRAY); // Cubemap array not supported
							pvrFile.read(buffer, curMipBytes);
							uint32_t depth = mipSize.z * header.numSurfaces;
							glTexImage3D(glTarget, mipLevel, internalFormat, mipSize.x, mipSize.y, depth, 0, sourceFormat, dataType, buffer);
							mipSize.z = max(1u, mipSize.z >> 1);
						}
						mipSize.y = max(1u, mipSize.y >> 1);
					}
					mipSize.x = max(1u, mipSize.x >> 1);
				}
				// Release temp buffer
				delete[] buffer;
				return true;
			}
		}
		
		//--------------------------------------------------------------------------------------------------------------
		Texture::Texture(const math::Vec2u& _size, InternalFormat _targetFormat, bool _multiSample)
			: mMultiSample(_multiSample)
		{
#ifndef ANDROID
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
			else
#endif // ANDROID
			{
				GLenum srcFormat = (GLenum)_targetFormat;
				if (_targetFormat == InternalFormat::rgb)
					srcFormat = GL_RGB;
				else if (_targetFormat == InternalFormat::rgba)
					srcFormat = GL_RGBA;
				if (_targetFormat == InternalFormat::rgb)
					srcFormat = GL_RGB;
#ifndef ANDROID
				else if (_targetFormat == InternalFormat::r)
					srcFormat = GL_RED;
				else if (_targetFormat == InternalFormat::rg)
					srcFormat = GL_RG;
				else if (_targetFormat == InternalFormat::rg32f)
					srcFormat = GL_RG;
				else if (_targetFormat == InternalFormat::rg16f)
					srcFormat = GL_RG;
#endif // ANDROID
				if(_targetFormat == InternalFormat::depth) {
					glTexImage2D(GL_TEXTURE_2D, 0, (GLint)_targetFormat, _size.x, _size.y, 0, (GLint)_targetFormat, GL_FLOAT, nullptr);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					// Remove artefact on the edges of the shadowmap
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				}
				else {
					glTexImage2D(GL_TEXTURE_2D, 0, (GLint)_targetFormat, _size.x, _size.y, 0, srcFormat, GL_UNSIGNED_BYTE, nullptr);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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
		Texture::Texture(const TextureInfo& _desc, const ImageBuffer* _buffers, size_t nMaps)
			: mInfo(_desc)
			, mMultiSample(false)
		{
			constexpr int borderSize = 0;
			// Generate OpenGL texture buffer
			glGenTextures(1, &mId);
			if(_desc.type == TexType::tex2d) {
				glBindTexture(GL_TEXTURE_2D, mId);

				if(_desc.genMips)
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				else
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				GLint repeatMode = _desc.repeat?GL_REPEAT: GL_CLAMP_TO_EDGE;
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeatMode);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeatMode);

				// Load mipmaps
				for (size_t mipLevel = 0; mipLevel < nMaps; ++mipLevel) {
					const ImageBuffer& buffer = _buffers[mipLevel];
					glTexImage2D(GL_TEXTURE_2D, mipLevel, (GLint)_desc.gpuFormat,
						_desc.size.x, _desc.size.y, borderSize, (GLenum)buffer.fmt, GL_UNSIGNED_BYTE, buffer.data);
				}
				if (_desc.genMips)
					glGenerateMipmap(GL_TEXTURE_2D);
			}
			else if (_desc.type == TexType::cubemap) {
				glBindTexture(GL_TEXTURE_CUBE_MAP, mId);
				// Config filtering
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

				// Load faces
				for(size_t face = 0; face < nMaps; ++face) {
					const ImageBuffer& buffer = _buffers[face];
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
						0, (GLint)_desc.gpuFormat, _desc.size.x, _desc.size.y, borderSize, (GLenum)buffer.fmt, GL_UNSIGNED_BYTE, buffer.data);
					
				}
			}
		}

		//--------------------------------------------------------------------------------------------------------------
		Texture* Texture::loadFromFile(const std::string& _fileName) {
			if (util::getFileExtension(_fileName) != "pvr") {
				cout << "Error loading texture " << _fileName << ". Only PVR textures are supported\n";
				return nullptr;
			}
			ifstream pvrFile(_fileName, fstream::binary);
			if (!pvrFile.is_open()) {
				cout << "Error: unable to open PVR file " << _fileName << "\n";
				return nullptr;
			}

			PVRHeader header;
			pvrFile.read((char*)&header, 52);
			if (!header.checkHeader()) {
				cout << "Error loading PVR file " << _fileName << "\n";
				return nullptr;
			}
			TextureInfo tInfo;
			tInfo.gpuFormat = header.gpuFormat();
			tInfo.size = math::Vec3u(header.width, header.height, header.depth);
			tInfo.type = header.numFaces == 6 ? Texture::TexType::cubemap : Texture::TexType::tex2d;

			// Skip metadata
			char buffer[1024];
			if (header.metaDataSize > 0) {
				assert(header.metaDataSize <= 1024);
				pvrFile.read(buffer, header.metaDataSize);
			}
			// Generate GL texture
			Texture* texture = new Texture();
			glGenTextures(1, &texture->mId);
			GLenum glTarget = header.glTarget();
			texture->mInfo.type = (glTarget == GL_TEXTURE_CUBE_MAP ? Texture::TexType::cubemap : Texture::TexType::tex2d);
			glBindTexture(glTarget, texture->mId);
			// Load actual data
			if (!loadPVRImageToGL(pvrFile, tInfo, header, texture->mId, glTarget)) {
				glDeleteTextures(1, &texture->mId);
				delete texture;
				return nullptr;
			}
			glTexParameteri(glTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(glTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(glTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(glTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(glTarget, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(glTarget, GL_TEXTURE_MAX_LEVEL, header.mipMapCount - 1);
			glBindTexture(glTarget, 0);
			return texture;
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
#ifndef ANDROID
			else if(_s == "rg")
				fmt = InternalFormat::rg;
			else if (_s == "r")
				fmt = InternalFormat::r;
#endif // ANDROID
			return fmt;
		}
	}
}