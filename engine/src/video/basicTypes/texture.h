//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
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
			enum class EImageFormat {
				rgb,
				rgba,
				alpha,
				luminance,
				lumiAlpha,
				depth,
				depthStencil
			};

			enum class EByteFormat {
				eUnsignedByte,
				eByte,
				eUnsignedShort,
				eShort,
				eUnsignedInt,
				eInt,
				eFloat
			};

		public:
			Texture(const math::Vec2u& _size, Texture::EImageFormat _if, Texture::EByteFormat _bf, void* _data = nullptr, bool _multiSample = false);
			~Texture();

			static Texture* load(const std::string& _fileName, GraphicsDriver* _driver);

			bool	multiSample	() const { return mMultiSample; }
			GLuint	glId		() const { return mId; }

			math::Vec2u size;
			uint8_t * data = nullptr;
			EImageFormat imgFormat;
			EByteFormat byteFormat;

		private:
			static GLint enumToGl(EImageFormat _format);
			static GLint enumToGl(EByteFormat _format);

			bool mMultiSample = false;
			GLuint mId;
		};
	}
}

#endif // _REV_VIDEO_BASICTYPES_TEXTURE_H_
