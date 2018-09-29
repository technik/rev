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
#include <iostream>
#include <string>

using namespace std;

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
		// Validate input data
		assert((descriptor.srcImages && descriptor.providedImages)
			|| (!descriptor.srcImages && !descriptor.providedImages));
		assert(descriptor.providedImages <= descriptor.mipLevels);
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
			auto& mip0 = descriptor.srcImages[0];
			switch(mip0.format().numChannels)
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
		if(descriptor.depth)
		{
			assert(descriptor.pixelFormat.numChannels == 1);
			imageFormat = GL_DEPTH;
		}
		// TODO: Maybe using glTextureStorage2D is simpler and faster for the case where images are not provided
		GLenum internalFormat = getInternalFormat(descriptor.pixelFormat, descriptor.sRGB);
		// Submit provided images to the device
		for(size_t i = 0; i < descriptor.providedImages; ++i)
		{
			auto& image = descriptor.srcImages[i];
			GLenum srcChannelType = image.format().channel == Image::ChannelFormat::Float32 ? GL_FLOAT : GL_UNSIGNED_BYTE;
			assert(image.format() == descriptor.srcImages[0].format());
			assert(image.size() == mipSize);
			glTexImage2D(GL_TEXTURE_2D, i,
				internalFormat,
				(GLsizei)mipSize.x(), (GLsizei)mipSize.y(), 0,
				imageFormat,
				srcChannelType,
				descriptor.srcImages[i].data<void>());
			mipSize = {
				std::max(mipSize.x()/2, 1u),
				std::max(mipSize.y()/2, 1u)
			};
		}
		// Allocate empty images to complete the mip chain
		for(size_t i = descriptor.providedImages; i < descriptor.mipLevels; ++i)
		{
			glTexImage2D(GL_TEXTURE_2D, i,
				internalFormat,
				(GLsizei)mipSize.x(), (GLsizei)mipSize.y(), 0,
				imageFormat,
				GL_UNSIGNED_BYTE,
				nullptr);
		}
		// Set mip level bounds
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, descriptor.mipLevels-1);
		// Generate mipmaps when needed
		if(descriptor.providedImages < descriptor.mipLevels)
			glGenerateTextureMipmap(textureName);

		texture.id = textureName;
		return texture;
	}

	//----------------------------------------------------------------------------------------------
	void DeviceOpenGL::destroyTexture2d(Texture2d texture)
	{
		if(texture.id == Texture2d::InvalidId)
			return;
		GLuint textureName = texture.id;
		glDeleteTextures(1, &textureName);
	}

	//----------------------------------------------------------------------------------------------
	FrameBuffer DeviceOpenGL::createFrameBuffer(const FrameBuffer::Descriptor& desc)
	{
		FrameBuffer newFb;
		GLuint fbId;
		glGenFramebuffers(1, &fbId);

		// Use descriptor info to bind resources
		glBindFramebuffer(GL_FRAMEBUFFER, fbId);

		int nColorAttachs = 0;
		for(size_t i = 0; i < desc.numAttachments; ++i)
		{
			auto& attachment = desc.attachments[i];

			// Color vs depth attachment
			GLenum attachTarget = GL_DEPTH_ATTACHMENT;
			if(attachment.target == FrameBuffer::Attachment::Color)
				attachTarget = GL_COLOR_ATTACHMENT0 + nColorAttachs++;

			// Bind attachment to an attachment point
			//if(attachment.imageType == FrameBuffer::Attachment::Texture)
				glFramebufferTexture2D(GL_FRAMEBUFFER, attachTarget, GL_TEXTURE_2D, attachment.texture.id, attachment.mipLevel);
			//else // TODO: Render buffer
				//glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachTarget, GL_RENDERBUFFER, attachment.RenderBuffer.id);
		}

		// If configuration was successful, return the new frame buffer
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		{
			newFb.id = int32_t(fbId);
		}

		// Unbind fb to prevent state leaks
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return newFb;
	}

	//----------------------------------------------------------------------------------------------
	Pipeline::ShaderModule DeviceOpenGL::createShaderModule(const Pipeline::ShaderModule::Descriptor& desc)
	{
		Pipeline::ShaderModule shader;
		std::vector<const char*> code;
		for(auto& c : desc.code)
		{
			code.push_back(c.c_str());
		}
		auto shaderId = glCreateShader(desc.stage == Pipeline::ShaderModule::Descriptor::Vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
		glShaderSource(shaderId, code.size(), code.data(), nullptr);
		glCompileShader(shaderId);

		GLint result = GL_FALSE;
		int InfoLogLength = 0;
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if ( GL_FALSE == result ){
			std::vector<char> ShaderErrorMessage(InfoLogLength+1);
			glGetShaderInfoLog(shaderId, InfoLogLength, NULL, &ShaderErrorMessage[0]);
			cout << "//----------------------------------------------------------------------------------------------\n";
			cout << "DeviceOpenGL::createShaderModule Error\n";
			cout << "//----------------------------------------------------------------------------------------------\n";
			std::string textMessage = (char*)ShaderErrorMessage.data();
			cout << textMessage << "\n";
			cout << "//----------------------------------------------------------------------------------------------\n";
			cout << "Program code" << "\n";
			cout << "//----------------------------------------------------------------------------------------------\n";
			glGetShaderiv(shaderId, GL_SHADER_SOURCE_LENGTH, &InfoLogLength);
			ShaderErrorMessage.resize(InfoLogLength+1);
			glGetShaderSource(shaderId, InfoLogLength, NULL, &ShaderErrorMessage[0]);
			std::string completeSource = (char*)ShaderErrorMessage.data();
			cout << completeSource << "\n";
		}
		else
			shader.id = shaderId;

		return shader;
	}

	//----------------------------------------------------------------------------------------------
	Pipeline DeviceOpenGL::createPipeline(const Pipeline::Descriptor& desc)
	{
		assert(desc.vtxShader.id != Pipeline::InvalidId);
		assert(desc.pxlShader.id != Pipeline::InvalidId);
		Pipeline pipeline;
		// Link the program
		auto program = glCreateProgram();
		glAttachShader(program, desc.vtxShader.id);
		glAttachShader(program, desc.pxlShader.id);
		glLinkProgram(program);

		// Check the program
		GLint result = GL_FALSE;
		int InfoLogLength = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &result);
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (GL_FALSE == result){
			std::vector<char> ShaderErrorMessage(InfoLogLength+1);
			glGetProgramInfoLog(program, InfoLogLength, NULL, &ShaderErrorMessage[0]);
			cout << "//----------------------------------------------------------------------------------------------\n";
			cout << "DeviceOpenGL::createPipeline Error\n";
			cout << "//----------------------------------------------------------------------------------------------\n";
			std::string textMessage = (char*)ShaderErrorMessage.data();
			cout << textMessage << "\n";
			cout << "//----------------------------------------------------------------------------------------------\n";
			cout << "Program code" << "\n";
			cout << "//----------------------------------------------------------------------------------------------\n";
			glGetShaderiv(program, GL_SHADER_SOURCE_LENGTH, &InfoLogLength);
			ShaderErrorMessage.resize(InfoLogLength+1);
			glGetShaderSource(program, InfoLogLength, NULL, &ShaderErrorMessage[0]);
			std::string completeSource = (char*)ShaderErrorMessage.data();
			cout << completeSource << "\n";
		}
		else
		{
			pipeline.id = m_pipelines.size();
			m_pipelines.push_back({desc, program});
		}
		return pipeline;
	}

	//----------------------------------------------------------------------------------------------
	GLenum DeviceOpenGL::getInternalFormat(
		Image::PixelFormat pixelFmt,
		bool sRGB)
	{
		// Detect sRGB cases
		if( pixelFmt.channel == Image::ChannelFormat::Byte)
		{
			switch(pixelFmt.numChannels)
			{
				case 1:
					return GL_R;
				case 2:
					return GL_RG;
				case 3:
					return sRGB ? GL_SRGB : GL_RGB8;
				case 4:
					return sRGB ? GL_SRGB_ALPHA : GL_RGBA;
				default:
					assert(!"Images must have [1,4] channels");
			}
		}
		else // Float formats
		{
			switch(pixelFmt.numChannels)
			{
				case 1:
					return GL_R;
				case 2:
					return GL_RG;
				case 3:
					return GL_RGB8;
				case 4:
					return GL_RGBA;
				default:
					assert(!"Images must have [1,4] channels");
			}
		}
		// By default, just cast back to OpenGL
		assert(!"Unsupported pixel format");
		return (GLint)GL_INVALID_ENUM;
	}

	//----------------------------------------------------------------------------------------------
	void DeviceOpenGL::bindPipeline(int32_t pipelineId)
	{
		const auto& pipeline = m_pipelines[pipelineId];
		glUseProgram(pipeline.nativeName);

		// Fixed function
		auto& desc = pipeline.desc;
		// Depth tests
		if(Pipeline::Descriptor::DepthTest::None == desc.depthTest)
		{
			glDisable(GL_DEPTH_TEST);
		}
		else
		{
			glEnable(GL_DEPTH_TEST);
			switch(desc.depthTest)
			{
				case Pipeline::Descriptor::DepthTest::Lequal:
					glDepthFunc(GL_LEQUAL);
					break;
				case Pipeline::Descriptor::DepthTest::Gequal:
					glDepthFunc(GL_GEQUAL);
					break;
			}
		}
		// Culling
		if(desc.cullFront || desc.cullBack)
		{
			glEnable(GL_CULL_FACE);
			if(desc.cullBack && desc.cullFront)
				glCullFace(GL_FRONT_AND_BACK);
			else if(desc.cullBack)
				glCullFace(GL_BACK);
			else
				glCullFace(GL_FRONT);
		}
		else
			glDisable(GL_CULL_FACE);
		// Front face winding
		if(desc.frontFace == Pipeline::Descriptor::CW)
			glFrontFace(GL_CW);
		else
			glFrontFace(GL_CCW);
	}
}