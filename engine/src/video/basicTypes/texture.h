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
			Texture(const math::Vec2u& _size, InternalFormat _targetFormat, SourceFormat _srcFormat, uint8_t* _data);
			// Empty texture
			Texture(const math::Vec2u& _size, InternalFormat _if, bool _multiSample = false);
			~Texture();

			static Texture* load(const std::string& _fileName, GraphicsDriver* _driver);

			// Texture options
			// Set wrapping
			// Set filtering
			// Set mipmaping
			void enableMipMaps	();
			void generateMipMaps();

			bool	multiSample	() const { return mMultiSample; }
			GLuint	glId		() const { return mId; }

			const math::Vec2u& size() const { return mSize; }

		private:
			math::Vec2u mSize;
			GLuint mId = 0;
			bool mMultiSample = false;
		};
	}
}

#endif // _REV_VIDEO_BASICTYPES_TEXTURE_H_
