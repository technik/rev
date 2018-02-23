//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2018 Carmelo J Fdez-Aguera
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#pragma once

#include <graphics/driver/openGL/openGL.h>
#include <graphics/Image.h>
#include <string>

namespace rev { namespace graphics {

	class Texture
	{
	public:
		std::string name;

		Texture(const Image& image, bool sRGB)
		{
			glGenTextures(1, &mGLName);
			glBindTexture(GL_TEXTURE_2D, mGLName);

			auto format = texFormat(image);
			auto internalFormat = internalTexFormat(image, sRGB);
			auto dataType = (image.format() == Image::ChannelFormat::Float32) ? GL_FLOAT : GL_UNSIGNED_BYTE;
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image.size().x(), image.size().y(), 0, format, dataType, image.data());

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glGenerateMipmap(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, 0);
		}

		~Texture()
		{
			glDeleteTextures(1, &mGLName);
		}

		static std::shared_ptr<Texture> load(const std::string _name, bool sRGB = true, unsigned nChannels = 0)
		{
			auto img = Image::load(_name, nChannels);
			if(!img)
				return nullptr;
			auto tex = std::make_shared<Texture>(*img, sRGB);
			tex->name = _name;
			return tex;
		}

		// Accessors
		auto glName() const { return mGLName; }

	private:
		static GLenum internalTexFormat(const Image& image, bool sRGB)
		{
			bool hdr = image.format() == Image::ChannelFormat::Float32;
			switch(image.nChannels())
			{
				case 1:
					return hdr?GL_R32F:GL_R8;
				case 2:
					return hdr?GL_RG32F:GL_RG8;
				case 3:
					return hdr?GL_RGB32F:(sRGB?GL_SRGB8:GL_RGB8);
				case 4:
					return hdr?GL_RGBA32F:(sRGB?GL_SRGB8_ALPHA8:GL_RGBA8);
			}
			return hdr?GL_R32F:GL_R8;
		}

		static GLenum texFormat(const Image& image)
		{
			bool hdr = image.format() == Image::ChannelFormat::Float32;
			switch(image.nChannels())
			{
				case 1:
					return GL_RED;
				case 2:
					return GL_RG;
				case 3:
					return GL_RGB;
				case 4:
					return GL_RGBA;
			}
			return GL_RED;
		}

		GLuint mGLName = 0;
	};

}}
