//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Textures
#pragma once

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
				lumin_alpha = GL_LUMINANCE_ALPHA,
#else
				r = GL_RED,
				rg = GL_RG8,
				rgb = GL_RGB8,
				rgba = GL_RGBA8,
				rg16f = GL_RG16F,
				rg32f = GL_RG32F,
				rgb32f = GL_RGB32F,
				rgba32f = GL_RGBA32F,
#endif // !ANDROID
				depth = GL_DEPTH_COMPONENT
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

			enum class TexType {
				tex2d,
				cubemap
			};

		public:
			// Empty texture, use this for render targets
			Texture(const math::Vec2u& _size, InternalFormat _if, bool _multiSample = false);
			~Texture();

			static Texture* loadFromFile(const std::string& _fileName);

			bool	multiSample	() const { return mMultiSample; }
			GLuint	glId		() const { return mId; }
			TexType	type		() const { return mInfo.type; }

			const math::Vec3u& size() const { return mInfo.size; }

			struct TextureInfo {
				TexType	type = TexType::tex2d;
				InternalFormat gpuFormat = InternalFormat::rgba;
				bool genMips = false;
				bool repeat = false;
				FilterMode filter = FilterMode::linear;
				math::Vec3u size;
			} mInfo;

		private:
			Texture();
			struct ImageBuffer{
				uint8_t* data = nullptr;
				math::Vec3u size;
				SourceFormat fmt;
			};

			enum class FilterMode {
				nearest = GL_NEAREST,
				linear = GL_LINEAR,
			};

			static FilterMode filterMode(const std::string&);
			static InternalFormat colorBufferFormat(const std::string&);
			Texture(const TextureInfo&, const ImageBuffer* _buffArray, size_t _nMips);

			GLuint mId = 0;
			bool mMultiSample = false;
		};
	}
}