//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Textures
#ifndef _REV_VIDEO_BASICTYPES_TEXTURE_H_
#define _REV_VIDEO_BASICTYPES_TEXTURE_H_

#include <video/graphics/driver/openGL/openGL.h>
#include <video/graphics/driver/graphicsDriver.h>
#include <math/algebra/vector.h>
#include <string>

namespace rev {
	namespace video {

		class Texture {
		public:
			enum class InternalFormat {
#ifdef ANDROID
				a = GL_ALPHA,
				rgb = GL_RGB,
				rgba = GL_RGBA,
				luminance = GL_LUMINANCE,
				lumin_alpha = GL_LUMINANCE_ALPHA
#else
				r = GL_RED,
				rg = GL_RG8,
				rgb = GL_RGB8,
				rgba = GL_RGBA8,
				rg16f = GL_RG16F,
				rg32f = GL_RG32F,
				depth = GL_DEPTH_COMPONENT
#endif // !ANDROID
			};

			enum class SourceFormat {
#ifdef ANDROID
				a = GL_ALPHA,
				rgb = GL_RGB,
				rgba = GL_RGBA,
				luminance = GL_LUMINANCE,
				lumin_alpha = GL_LUMINANCE_ALPHA
#else
				r = GL_RED,
				rg = GL_RG,
				rgb = GL_RGB,
				rgba = GL_RGBA,
				depth = GL_DEPTH,
				depthStencil = GL_DEPTH_STENCIL
#endif
			};

		public:
			// Empty texture, use this for render targets
			Texture(const math::Vec2u& _size, InternalFormat _if, bool _multiSample = false);
			~Texture();

			static Texture* loadFromFile(const std::string& _fileName);

			bool	multiSample	() const { return mMultiSample; }
			GLuint	glId		() const { return mId; }

			const math::Vec2u& size() const { return mSize; }

		private:
			struct ImageBuffer{
				uint8_t* data = nullptr;
				math::Vec2u size;
				SourceFormat fmt;
			};


			enum class FilterMode {
				nearest = GL_NEAREST,
				linear = GL_LINEAR,
			};

			struct TextureInfo {
				InternalFormat gpuFormat;
				bool genMips;
				bool repeat;
				FilterMode filter;
				math::Vec2u size;
			} mInfo;


			static FilterMode filterMode(const std::string&);
			static InternalFormat colorBufferFormat(const std::string&);
			static bool loadBuffer(const std::string& _fileName, ImageBuffer& _buff);
			Texture(const TextureInfo&, const ImageBuffer* _buffArray, size_t _nMips);

			math::Vec2u mSize;
			GLuint mId = 0;
			bool mMultiSample = false;
		};
	}
}

#endif // _REV_VIDEO_BASICTYPES_TEXTURE_H_
