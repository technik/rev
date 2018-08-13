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
#include "texture.h"

namespace rev::graphics
{
	//----------------------------------------------------------------------------------------------
	Texture::Texture(const Image& image, bool sRGB, const SamplerOptions& samplerInfo)
	{
		glGenTextures(1, &mGLName);
		glBindTexture(GL_TEXTURE_2D, mGLName);

		auto format = texFormat(image);
		auto internalFormat = internalTexFormat(image, sRGB);
		auto dataType = (image.format() == Image::ChannelFormat::Float32) ? GL_FLOAT : GL_UNSIGNED_BYTE;
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image.size().x(), image.size().y(), 0, format, dataType, image.data());

		setSamplingInfo(samplerInfo);

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	//----------------------------------------------------------------------------------------------
	Texture::Texture(const std::vector<std::shared_ptr<const Image>>& mips, bool sRGB, const SamplerOptions& samplerInfo)
	{
		glGenTextures(1, &mGLName);
		glBindTexture(GL_TEXTURE_2D, mGLName);

		auto image = mips[0];
		auto format = texFormat(*image);
		auto internalFormat = internalTexFormat(*image, sRGB);
		auto dataType = (image->format() == Image::ChannelFormat::Float32) ? GL_FLOAT : GL_UNSIGNED_BYTE;

		for(size_t i = 0; i < mips.size(); ++i)
		{
			image = mips[i];
			glTexImage2D(GL_TEXTURE_2D, i, internalFormat, image->size().x(), image->size().y(), 0, format, dataType, image->data());
			setSamplingInfo(samplerInfo);
		}

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	//----------------------------------------------------------------------------------------------
	Texture::~Texture()
	{
		if(mGLName)
			glDeleteTextures(1, &mGLName);
	}

	//----------------------------------------------------------------------------------------------
	std::shared_ptr<Texture> Texture::depthTexture(const math::Vec2u& size)
	{
		auto texture = std::make_shared<Texture>();

		glGenTextures(1, &texture->mGLName);
		glBindTexture(GL_TEXTURE_2D, texture->mGLName);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
			size.x(), size.y(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 

		return texture;
	}

	//----------------------------------------------------------------------------------------------
	std::shared_ptr<Texture> Texture::load(const std::string _name, bool sRGB, unsigned nChannels, const SamplerOptions& samplerInfo)
	{
		auto img = Image::load(_name, nChannels);
		if(!img)
		{
			core::Log::error("Unable to load texture:\n");
			core::Log::error(_name);
			return nullptr;
		}
		auto tex = std::make_shared<Texture>(*img, sRGB, samplerInfo);
		tex->name = _name;
		return tex;
	}

	//----------------------------------------------------------------------------------------------
	GLenum Texture::internalTexFormat(const Image& image, bool sRGB)
	{
		// TODO: Accordint to
		// https://www.khronos.org/registry/OpenGL-Refpages/es3/html/glTexImage2D.xhtml
		// Only certain internal formats support mipmap generation.
		// Maybe we could choose better internal formats for cases where mipmaps are not needed.
		bool hdr = image.format() == Image::ChannelFormat::Float32;
		switch(image.nChannels())
		{
			case 1:
				return hdr?GL_R16F:GL_R8;
			case 2:
				return hdr?GL_RG16F:GL_RG8;
			case 3:
				return hdr?GL_RGBA16F:(sRGB?GL_SRGB8_ALPHA8:GL_RGB8);
				//return hdr?GL_RGBA32F:(sRGB?GL_SRGB8:GL_RGB8);
				//return hdr?GL_RGBA16F:(GL_RGB8);
			case 4:
				return hdr?GL_RGBA32F:(sRGB?GL_SRGB8_ALPHA8:GL_RGBA8);
				//return hdr?GL_RGBA16F:(GL_RGBA8);
		}
		return hdr?GL_R32F:GL_R8;
	}

	//----------------------------------------------------------------------------------------------
	GLenum Texture::texFormat(const Image& image)
	{
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

	//----------------------------------------------------------------------------------------------
	void Texture::setSamplingInfo(const SamplerOptions& samplerInfo)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)samplerInfo.filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)samplerInfo.wrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)samplerInfo.wrapT);
	}
}