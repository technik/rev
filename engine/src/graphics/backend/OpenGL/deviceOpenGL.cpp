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
#include "renderQueueOpenGL.h"

using namespace std;

namespace rev :: gfx
{
	//----------------------------------------------------------------------------------------------
	TextureSampler DeviceOpenGL::createTextureSampler(const TextureSampler::Descriptor& desc)
	{
		m_textureSamplers.push_back(desc);
		TextureSampler sampler;
		sampler.id = int32_t(m_textureSamplers.size()-1);
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
		assert(descriptor.srcImages.size() <= descriptor.mipLevels
			|| descriptor.mipLevels == 0);
		assert(descriptor.mipLevels > 0 || descriptor.srcImages.size() > 0);
		Texture2d texture;
		// Validate input data
		// Generate opengl object
		GLuint textureName;
		GLenum textureTarget = descriptor.nFaces == 1 ? GL_TEXTURE_2D : GL_TEXTURE_CUBE_MAP;
		glGenTextures(1, &textureName);
		glBindTexture(textureTarget, textureName);
		// Locate sampler
		auto& sampler = m_textureSamplers[descriptor.sampler.id];
		// Setup sampler options
		glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, (GLenum)sampler.wrapS);
		glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, (GLenum)sampler.wrapT);
		if(descriptor.nFaces == 6)
			glTexParameteri(textureTarget, GL_TEXTURE_WRAP_R, (GLenum)sampler.wrapT);
		glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, (GLenum)sampler.filter);
		glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Allocate GPU side storage
		assert(descriptor.nFaces == 1 || descriptor.nFaces == 6);
		GLenum internalFormat = getInternalFormat(descriptor);
		size_t numProvidedMips = descriptor.srcImages.size() / descriptor.nFaces;
		assert(descriptor.nFaces * numProvidedMips == descriptor.srcImages.size());

		if (numProvidedMips == 0)
		{
			glTexStorage2D(
				textureTarget,
				descriptor.mipLevels, internalFormat,
				(GLsizei)descriptor.size.x(), (GLsizei)descriptor.size.y());
		}
		else
		{
			// Submit provided images to the device
			math::Vec2u mipSize = descriptor.size;
			GLenum imageFormat = getImageFormat(descriptor);
			GLenum srcChannelType = descriptor.pixelFormat.channel == Image::ChannelFormat::Float32 ? GL_FLOAT : GL_UNSIGNED_BYTE;
			for (size_t level = 0; level < numProvidedMips; ++level)
			{
				for (size_t face = 0; face < descriptor.nFaces; ++face)
				{
					auto& image = descriptor.srcImages[level * descriptor.nFaces + face];
					assert(image->format() == descriptor.pixelFormat);
					assert(image->size() == mipSize);

					GLenum faceTarget = GL_TEXTURE_2D;
					if (descriptor.nFaces == 6)
						faceTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X + face;

					glTexImage2D(faceTarget, level,
						internalFormat,
						(GLsizei)mipSize.x(), (GLsizei)mipSize.y(), 0,
						imageFormat,
						srcChannelType,
						image->data<void>());
				}
				// Compute size of next mip level
				mipSize = {
					std::max(mipSize.x() / 2, 1u),
					std::max(mipSize.y() / 2, 1u)
				};
			}
		}

		// Set mip level bounds
		if(descriptor.mipLevels != 0)
			glTexParameteri(textureTarget, GL_TEXTURE_MAX_LEVEL, descriptor.mipLevels-1);
		// Generate mipmaps when needed
		if ((descriptor.srcImages.size() && (descriptor.srcImages.size() < descriptor.mipLevels * descriptor.nFaces))
			|| descriptor.mipLevels == 0)
		{
			glTexParameteri(textureTarget, GL_TEXTURE_BASE_LEVEL, numProvidedMips);
			glGenerateMipmap(textureTarget);
			glTexParameteri(textureTarget, GL_TEXTURE_BASE_LEVEL, 0);
		}
		return Texture2d(textureName);
	}

	//----------------------------------------------------------------------------------------------
	void DeviceOpenGL::destroyTexture2d(Texture2d texture)
	{
		if(!texture.isValid())
			return;
		GLuint textureName = texture.id();
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

		FrameBufferInfo fbInfo;
		fbInfo.sRGB = desc.sRGB;
		fbInfo.numColorAttachs = 0;
		for(size_t i = 0; i < desc.numAttachments; ++i)
		{
			auto& attachment = desc.attachments[i];
			// Color vs depth attachment
			GLenum attachTarget = GL_DEPTH_ATTACHMENT;
			if (attachment.target == FrameBuffer::Attachment::Color)
			{
				attachTarget = GL_COLOR_ATTACHMENT0 + fbInfo.numColorAttachs;
				fbInfo.colorAttachments[fbInfo.numColorAttachs] = attachTarget;
				fbInfo.numColorAttachs++;
			}

			// Bind attachment to an attachment point
			if (attachment.imageType == FrameBuffer::Attachment::Texture)
			{
				if(attachment.side == Texture2d::CubeMapSide::None)
					glFramebufferTexture2D(GL_FRAMEBUFFER, attachTarget, GL_TEXTURE_2D, attachment.texture.id(), attachment.mipLevel);
				else
				{
					auto targetSide = GL_TEXTURE_CUBE_MAP_POSITIVE_X + (GLenum)attachment.side;
					glFramebufferTexture2D(GL_FRAMEBUFFER, attachTarget, targetSide, attachment.texture.id(), attachment.mipLevel);
				}
			}
			else
			{
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachTarget, GL_RENDERBUFFER, attachment.texture.id());
			}
		}

		// If configuration was successful, return the new frame buffer
		auto fbState = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if(fbState == GL_FRAMEBUFFER_COMPLETE)
		{
			// allocate a new frame buffer
			fbInfo.frameBufferGLname = fbId;
			m_frameBuffers.push_back(fbInfo);
			newFb = FrameBuffer((int32_t)m_frameBuffers.size()); // Index +1, because 0 is reserved for the default fb
		}
		else
		{
			if (fbState == GL_FRAMEBUFFER_UNDEFINED)
				cout << "Framebuffer undefined\n";
			else if (fbState == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
				cout << "Framebuffer incomplete attachment\n";
			else if (fbState == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
				cout << "Missing framebuffer attachment\n";
			else if (fbState == GL_FRAMEBUFFER_UNSUPPORTED)
				cout << "Unsupported framebuffer format\n";
			else
				cout << "Incomplete framebuffer, other reasons\n";
			assert( false );
		}

		// Unbind fb to prevent state leaks
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return newFb;
	}

	//----------------------------------------------------------------------------------------------
	void DeviceOpenGL::destroyFrameBuffer(FrameBuffer fb)
	{
		assert(fb.isValid());
		assert(fb.id() > 0);
		auto& fbInfo = m_frameBuffers[fb.id()-1];
		assert(fbInfo.isValid);
		fbInfo.isValid = false;
		while(!m_frameBuffers.empty() && !m_frameBuffers.back().isValid)
			m_frameBuffers.pop_back();
	}

	//----------------------------------------------------------------------------------------------
	void DeviceOpenGL::bindFrameBuffer(FrameBuffer fb)
	{
		// Locate frame buffer information
		assert(fb.isValid());

		// Default frame buffer?
		if (fb.id() == 0)
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glEnable(GL_FRAMEBUFFER_SRGB);
			return;
		}

		// Bind the frame buffer
		assert(fb.id() > 0);
		auto& fbInfo = m_frameBuffers[fb.id() - 1];
		assert(fbInfo.isValid);
		glBindFramebuffer(GL_FRAMEBUFFER, fbInfo.frameBufferGLname);
		glDrawBuffers(fbInfo.numColorAttachs, fbInfo.colorAttachments);
		if (fbInfo.sRGB)
			glEnable(GL_FRAMEBUFFER_SRGB);
	}

	//----------------------------------------------------------------------------------------------
	void DeviceOpenGL::bindPass(int32_t pass, RenderQueue& queue)
	{
		m_passes[pass]->bindTo(static_cast<RenderQueueOpenGL&>(queue));
	}

	//----------------------------------------------------------------------------------------------
	RenderPass* DeviceOpenGL::createRenderPass(const RenderPass::Descriptor& desc)
	{
		auto newPass = new RenderPassOpenGL(desc, desc.numColorAttachs, (int32_t)m_passes.size());
		m_passes.push_back(newPass);
		return newPass;
	}

	//----------------------------------------------------------------------------------------------
	auto splitLines(const std::string& openglErrorString)
	{
		std::vector<std::string_view>  errorMessages;
		size_t searchPos = 0;

		auto lineEnd = openglErrorString.find("\n");
		while(lineEnd != std::string::npos)
		{
			errorMessages.emplace_back(&openglErrorString[searchPos], lineEnd-searchPos);
			searchPos = lineEnd+1;
			lineEnd = openglErrorString.find("\n", searchPos);
		}

		if(searchPos < openglErrorString.size()-1)
		{
			errorMessages.emplace_back(&openglErrorString[searchPos]);
		}

		return errorMessages;
	}

	//----------------------------------------------------------------------------------------------
	void printShaderError(const std::string& openglErrorString, const std::string& code)
	{
		// Split error code into separate errors.
		std::vector<std::string_view>  errorMessages = splitLines(openglErrorString);

		std::vector<string_view> splitCode = splitLines(code);

		for(auto& line : errorMessages)
		{
			// Extract file and line info
			auto openParenPos = line.find('(');
			auto errorLineNumber = std::atoi(line.substr(openParenPos+1).data());
			
			std::cout << ">> " << line << "\n\n";
			int minLine = std::max(0, errorLineNumber-3);
			int maxLine = std::min<int>(splitCode.size(), errorLineNumber+3);
			for(int i = minLine; i < maxLine; ++i)
			{
				std::cout << "(" << i+1 << "): " << splitCode[i] << "\n";
			}
			std::cout << "\n";
		}
	}

	//----------------------------------------------------------------------------------------------
	Pipeline::ShaderModule DeviceOpenGL::createShaderModule(const Pipeline::ShaderModule::Descriptor& desc)
	{
		Pipeline::ShaderModule shader;
		std::vector<const char*> code;
		code.push_back("#version 450\n");
		if(desc.stage == Pipeline::ShaderModule::Descriptor::Vertex)
			code.push_back("#define VTX_SHADER\n");
		else
			code.push_back("#define PXL_SHADER\n");
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
			// Get error message
			std::vector<char> ShaderErrorMessage(InfoLogLength+1);
			glGetShaderInfoLog(shaderId, InfoLogLength, NULL, &ShaderErrorMessage[0]);
			std::string textMessage = (char*)ShaderErrorMessage.data();

			// Get source code
			glGetShaderiv(shaderId, GL_SHADER_SOURCE_LENGTH, &InfoLogLength);
			ShaderErrorMessage.resize(InfoLogLength+1);
			glGetShaderSource(shaderId, InfoLogLength, NULL, &ShaderErrorMessage[0]);
			std::string completeSource = (char*)ShaderErrorMessage.data();
			printShaderError(textMessage, completeSource);
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
		}
		else
		{
			pipeline.id = m_pipelines.size();
			m_pipelines.push_back({desc, program});
		}
		return pipeline;
	}

	//----------------------------------------------------------------------------------------------
	ComputeShader DeviceOpenGL::createComputeShader(const std::vector<std::string>& code)
	{
		std::vector<const char*> raw_code;
		raw_code.push_back("#version 430\n");
		for(auto& c : code)
		{
			raw_code.push_back(c.c_str());
		}
		GLuint shaderId = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(shaderId, raw_code.size(), raw_code.data(), nullptr);
		glCompileShader(shaderId);
		// check for compilation errors as per normal here
		GLint result = GL_FALSE;
		int infoLogLength = 0;
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
		if ( GL_FALSE == result ){
			// Get error message
			std::vector<char> ShaderErrorMessage(infoLogLength+1);
			glGetShaderInfoLog(shaderId, infoLogLength, NULL, &ShaderErrorMessage[0]);
			std::string textMessage = (char*)ShaderErrorMessage.data();

			// Get source code
			glGetShaderiv(shaderId, GL_SHADER_SOURCE_LENGTH, &infoLogLength);
			ShaderErrorMessage.resize(infoLogLength+1);
			glGetShaderSource(shaderId, infoLogLength, NULL, &ShaderErrorMessage[0]);
			std::string completeSource = (char*)ShaderErrorMessage.data();
			printShaderError(textMessage, completeSource);
		}

		GLuint programId = glCreateProgram();
		glAttachShader(programId, shaderId);
		glLinkProgram(programId);

		// Check the program
		result = GL_FALSE;
		infoLogLength = 0;
		glGetProgramiv(programId, GL_LINK_STATUS, &result);
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (GL_FALSE == result){
			std::vector<char> ShaderErrorMessage(infoLogLength+1);
			glGetProgramInfoLog(programId, infoLogLength, NULL, &ShaderErrorMessage[0]);
			cout << "//----------------------------------------------------------------------------------------------\n";
			cout << "DeviceOpenGL::createPipeline Error\n";
			cout << "//----------------------------------------------------------------------------------------------\n";
			std::string textMessage = (char*)ShaderErrorMessage.data();
			cout << textMessage << "\n";
			programId = -1; // Invalidate
		}
		return ComputeShader(programId);
	}

	//----------------------------------------------------------------------------------------------
	void DeviceOpenGL::destroyComputeShader(const ComputeShader& shader)
	{
		glDeleteProgram(shader.id());
	}

	//----------------------------------------------------------------------------------------------
	GLenum DeviceOpenGL::getImageFormat(const Texture2d::Descriptor& descriptor)
	{
		if (descriptor.depth)
		{
			assert(descriptor.pixelFormat.numChannels == 1);
			return GL_DEPTH_COMPONENT;
		}
		switch (descriptor.pixelFormat.numChannels)
		{
		case 1:
			return GL_R;
			break;
		case 2:
			return GL_RG;
			break;
		case 3:
			return GL_RGB;
			break;
		case 4:
			return GL_RGBA;
			break;
		default:
			assert(false && "Invalid number of channels");
		}
		return GL_RGBA;
	}

	//----------------------------------------------------------------------------------------------
	GLenum DeviceOpenGL::getInternalFormat(const Texture2d::Descriptor& desc)
	{
		if (desc.depth)
		{
			assert(desc.pixelFormat.channel == Image::ChannelFormat::Float32);
			return GL_DEPTH_COMPONENT32F;
		}
		// Detect sRGB cases
		if( desc.pixelFormat.channel == Image::ChannelFormat::Byte)
		{
			switch(desc.pixelFormat.numChannels)
			{
				case 1:
					return GL_R8;
				case 2:
					return GL_RG8;
				case 3:
					return desc.sRGB ? GL_SRGB8 : GL_RGB8;
				case 4:
					return desc.sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8;
				default:
					assert(!"Images must have [1,4] channels");
			}
		}
		else // Float formats
		{
			switch(desc.pixelFormat.numChannels)
			{
				case 1:
					return GL_R32F;
				case 2:
					return GL_RG32F;
				case 3:
					return GL_RGB32F;
				case 4:
					return GL_RGBA32F;
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
		assert(pipelineId != Pipeline::InvalidId);
		const auto& pipeline = m_pipelines[pipelineId];
		glUseProgram(pipeline.nativeName);

		// Fixed function
		auto& desc = pipeline.desc;
		// Alpha blending
		if (Pipeline::BlendMode::Write == desc.raster.blendMode)
		{
			glDisable(GL_BLEND);
		}
		else
		{
			// Additive
			assert(desc.raster.blendMode == Pipeline::BlendMode::Additive);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);
		}
		// Depth tests
		if(Pipeline::DepthTest::None == desc.raster.depthTest)
		{
			glDisable(GL_DEPTH_TEST);
		}
		else
		{
			if(desc.raster.witeDepth)
			{
				glDepthMask(GL_TRUE);
			}
			else
			{
				glDepthMask(GL_FALSE);
			}
			glEnable(GL_DEPTH_TEST);
			switch(desc.raster.depthTest)
			{
				case Pipeline::DepthTest::Lequal:
					glDepthFunc(GL_LEQUAL);
					break;
				case Pipeline::DepthTest::Gequal:
					glDepthFunc(GL_GEQUAL);
					break;
				case Pipeline::DepthTest::Less:
					glDepthFunc(GL_LESS);
					break;
				default:
					assert(false && "Unsupported depth function");
					return;
			}
		}
		// Culling
		if(desc.raster.cullFront || desc.raster.cullBack)
		{
			glEnable(GL_CULL_FACE);
			if(desc.raster.cullBack && desc.raster.cullFront)
				glCullFace(GL_FRONT_AND_BACK);
			else if(desc.raster.cullBack)
				glCullFace(GL_BACK);
			else
				glCullFace(GL_FRONT);
		}
		else
			glDisable(GL_CULL_FACE);
		// Front face winding
		if(desc.raster.frontFace == Pipeline::Winding::CW)
			glFrontFace(GL_CW);
		else
			glFrontFace(GL_CCW);
	}

	//----------------------------------------------------------------------------------------------
	auto DeviceOpenGL::allocateBuffer(size_t byteSize, BufferUpdateFrequency freq, BufferUsageTarget usage, const void* data) -> Buffer
	{
		GLuint glBufferHandle;
		GLenum glTarget = toGL(usage);
		glGenBuffers(1, &glBufferHandle);
		glBindBuffer(glTarget, glBufferHandle);
		glBufferData(glTarget, byteSize, data, toGL(freq));
		glBindBuffer(glTarget, 0);
		return Buffer(glBufferHandle);
	}

	//----------------------------------------------------------------------------------------------
	void* DeviceOpenGL::mapBuffer(Buffer buffer, BufferUsageTarget usage, size_t offset, size_t length)
	{
		GLenum glTarget = toGL(usage);
		glBindBuffer(glTarget, buffer.id());
		void* clientSideMemory = glMapBufferRange(glTarget, (GLintptr)offset, (GLsizeiptr)length, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
		glBindBuffer(glTarget, 0);
		return clientSideMemory;
	}

	//----------------------------------------------------------------------------------------------
	void DeviceOpenGL::unmapBuffer(Buffer buffer, BufferUsageTarget usage)
	{
		GLenum glTarget = toGL(usage);
		glBindBuffer(glTarget, buffer.id());
		glUnmapBuffer(glTarget);
		glBindBuffer(glTarget, 0);
	}

	//----------------------------------------------------------------------------------------------
	void DeviceOpenGL::readDeviceLimits()
	{
		// Read compute shader limits
		int groupCount[3];
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &groupCount[0]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &groupCount[1]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &groupCount[2]);
		m_deviceLimits.computeWorkGroupCount.x() = groupCount[0];
		m_deviceLimits.computeWorkGroupCount.y() = groupCount[1];
		m_deviceLimits.computeWorkGroupCount.z() = groupCount[2];

		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &groupCount[0]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &groupCount[1]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &groupCount[2]);
		m_deviceLimits.computeWorkGroupSize.x() = groupCount[0];
		m_deviceLimits.computeWorkGroupSize.y() = groupCount[1];
		m_deviceLimits.computeWorkGroupSize.z() = groupCount[2];

		glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &m_deviceLimits.computeWorkGruopTotalInvokes);
	}

	GLenum DeviceOpenGL::toGL(BufferUpdateFrequency freq)
	{
		switch (freq)
		{
		case BufferUpdateFrequency::Static:
			return GL_STATIC_DRAW;
		case BufferUpdateFrequency::Dynamic:
			return GL_DYNAMIC_DRAW;
		case BufferUpdateFrequency::Streamming:
			return GL_STREAM_DRAW;
		}
		assert(false && "Unsuported update frequency");
		return GL_STATIC_DRAW;
	}

	GLenum DeviceOpenGL::toGL(BufferUsageTarget usage)
	{
		switch (usage)
		{
		case BufferUsageTarget::Vertex:
			return GL_ARRAY_BUFFER;
		case BufferUsageTarget::Index:
			return GL_ELEMENT_ARRAY_BUFFER;
		case BufferUsageTarget::Uniform:
			return GL_UNIFORM_BUFFER;
		case BufferUsageTarget::ShaderStorage:
			return GL_SHADER_STORAGE_BUFFER;
		}
		assert(false && "Unsuported buffer target");
		return GL_SHADER_STORAGE_BUFFER;
	}
}