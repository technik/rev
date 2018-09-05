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
#include "deviceOpenGL.h"
#include <graphics/Image.h>
#include <math/linear.h>

namespace rev :: gfx
{
	//----------------------------------------------------------------------------------------------
	TextureSampler DeviceOpenGL::createTextureSampler(const TextureSampler::Descriptor& desc)
	{
		m_textureSamplers.push_back(desc);
		TextureSampler sampler;
		sampler.id = m_textureSamplers.size()-1;
		return sampler;
	}

	//----------------------------------------------------------------------------------------------
	void DeviceOpenGL::destroyTextureSampler(TextureSampler sampler)
	{
		// TODO: Fix this leak
		if(m_textureSamplers.size() == sampler.id+1)
		{
			m_textureSamplers.pop_back();
		}
	}

	//----------------------------------------------------------------------------------------------
	Texture2d DeviceOpenGL::createTexture2d(const Texture2d::Descriptor& descriptor)
	{
		Texture2d texture;
		// Validate input data
		// Generate opengl object
		GLuint textureName;
		glGenTextures(1, &textureName);
		glBindTexture(GL_TEXTURE_2D, textureName);
		// Locate sampler
		auto& sampler = m_textureSamplers[descriptor.sampler.id];
		// Setup sampler options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLenum)sampler.wrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLenum)sampler.wrapT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLenum)sampler.filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Allocate images
		math::Vec2u mipSize = descriptor.size;
		// Gather image format from mip 0
		GLenum imageFormat = GL_RGBA;
		if(descriptor.srcImages)
		{
			auto mip0 = descriptor.srcImages[0];
			switch(mip0->nChannels())
			{
				case 1:
					imageFormat = GL_R;
					break;
				case 2:
					imageFormat = GL_RG;
					break;
				case 3:
					imageFormat = GL_RGB;
					break;
				case 4:
					imageFormat = GL_RGBA;
					break;
				default:
					assert(false); // Invalid number of channels
					return texture;
			}
		}
		// TODO: Maybe using glTextureStorage2D is simpler and faster for the case where images are not provided
		size_t submittedLevels = 0;
		GLenum internalFormat = getInternalFormat(descriptor.channelType, descriptor.pixelFormat, descriptor.sRGB);
		for(size_t i = 0; i < descriptor.mipLevels; ++i)
		{
			if(descriptor.srcImages && descriptor.srcImages[i])
			{ // Send image to the GPU
				auto image = descriptor.srcImages[i];
				GLenum srcChannelType = image->format() == rev::graphics::Image::ChannelFormat::Float32 ? GL_FLOAT : GL_UNSIGNED_BYTE;
				assert(image->nChannels() == descriptor.srcImages[0]->nChannels());
				assert(image->format() == descriptor.srcImages[0]->format());
				assert(image->size() == mipSize);
				glTexImage2D(GL_TEXTURE_2D, i,
					internalFormat,
					(GLsizei)mipSize.x(), (GLsizei)mipSize.y(), 0,
					(GLenum)imageFormat,
					srcChannelType,
					descriptor.srcImages[i]->data());
				++submittedLevels;
			} else
			{ // Allocate an empty image
				glTexImage2D(GL_TEXTURE_2D, i,
					internalFormat,
					(GLsizei)mipSize.x(), (GLsizei)mipSize.y(), 0,
					(GLint)descriptor.pixelFormat,
					GL_UNSIGNED_BYTE,
					nullptr);
			}
			mipSize = {
				std::max(mipSize.x()/2, 1u),
				std::max(mipSize.y()/2, 1u)
			};
		}
		// Set mip level bounds
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, descriptor.mipLevels-1);
		// Generate mipmaps when needed
		if(submittedLevels < descriptor.mipLevels)
			glGenerateTextureMipmap(textureName);

		texture.id = textureName;
		return texture;
	}

	void DeviceOpenGL::destroyTexture2d(Texture2d texture)
	{
		if(texture.id == Texture2d::InvalidId)
			return;
		GLuint textureName = texture.id;
		glDeleteTextures(1, &textureName);
	}

	//----------------------------------------------------------------------------------------------
	GLenum DeviceOpenGL::getInternalFormat(
		Texture2d::Descriptor::ChannelType channel,
		Texture2d::Descriptor::PixelFormat pixel,
		bool sRGB)
	{
		if( channel == Texture2d::Descriptor::ChannelType::Float32)
		{
			if(pixel == Texture2d::Descriptor::PixelFormat::RGB)
				return sRGB ? GL_SRGB : GL_RGB8;
			else // RGBA
				return sRGB ? GL_SRGB_ALPHA : GL_RGBA;
		}
	}
}