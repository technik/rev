//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Textures
#ifndef _REV_VIDEO_BASICTYPES_TEXTURE_H_
#define _REV_VIDEO_BASICTYPES_TEXTURE_H_

#ifdef _WIN32
#include <Windows.h>
#include <video/graphics/driver/openGL/glew.h>
#else
#include <GL/glew.h>
#endif // !_WIN32

#include <GL/gl.h>

#include <math/algebra/vector.h>
#include <string>

namespace rev {
	namespace video {

		class GraphicsDriver;

		class Texture {
		public:
			enum class InternalFormat {
				r = GL_RED,
				rg = GL_RG8,
				rgb = GL_RGB8,
				rgba = GL_RGBA8,
				rg16f = GL_RG16F,
				rg32f = GL_RG32F,
				depth = GL_DEPTH_COMPONENT
			};

			enum class SourceFormat {
				r = GL_RED,
				rg = GL_RG,
				rgb = GL_RGB,
				rgba = GL_RGBA,
				depth = GL_DEPTH,
				depthStencil = GL_DEPTH_STENCIL
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
			GLuint mId = GL_INVALID_INDEX;
			bool mMultiSample = false;
		};
	}
}

#endif // _REV_VIDEO_BASICTYPES_TEXTURE_H_
