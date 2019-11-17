//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2019 Carmelo J Fdez-Aguera
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
#include "frameBufferCache.h"
#include <graphics/backend/device.h>

namespace rev::gfx {

	//--------------------------------------------------------------------------------------------------
	FrameBufferCache::FrameBufferCache(Device& gfxDevice)
		: m_device(gfxDevice)
	{
		TextureSampler::Descriptor samplerDesc;
		samplerDesc.filter = TextureSampler::MinFilter::Linear;
		samplerDesc.wrapS = TextureSampler::Wrap::Clamp;
		samplerDesc.wrapT = TextureSampler::Wrap::Clamp;
		m_bufferSampler = m_device.createTextureSampler(samplerDesc);
	}

	//--------------------------------------------------------------------------------------------------
	FrameBuffer FrameBufferCache::requestFrameBuffer(FrameBuffer::Descriptor desc)
	{
		for (auto& res : m_frameBuffers)
		{
			if (!res.locked && res.desc == desc)
			{
				res.locked = true;
				return res.handle;
			}
		}
		// No valid resource is free. Need to allocate another
		FBResource newResource;
		newResource.handle = m_device.createFrameBuffer(desc);
		newResource.locked = true;
		newResource.desc = desc;
		
		m_frameBuffers.push_back(newResource);
		return newResource.handle;
	}

	//--------------------------------------------------------------------------------------------------
	Texture2d FrameBufferCache::requestTargetTexture(BufferDesc desc)
	{
		for (auto& res : m_textures)
		{
			if (!res.locked && res.desc == desc)
			{
				res.locked = true;
				return res.handle;
			}
		}
		// No valid resource is free. Need to allocate another
		Texture2d::Descriptor textureDesc;
		textureDesc.depth = (desc.format == BufferFormat::depth24) || (desc.format == BufferFormat::depth32);
		textureDesc.mipLevels = 1;
		textureDesc.sampler = m_bufferSampler;
		textureDesc.size = desc.size;
		textureDesc.sRGB = desc.format == BufferFormat::sRGBA8;
		switch (desc.format)
		{
		case BufferFormat::RGBA32:
			textureDesc.pixelFormat.channel = Image::ChannelFormat::Float32;
			break;
		case BufferFormat::RGBA8:
		case BufferFormat::sRGBA8:
			textureDesc.pixelFormat.channel = Image::ChannelFormat::Byte;
			break;
		}
		textureDesc.pixelFormat.numChannels = textureDesc.depth ? 1 : 4;

		TextureResource newResource;
		newResource.handle = m_device.createTexture2d(textureDesc);
		newResource.locked = true;
		newResource.desc = desc;
		m_textures.push_back(newResource);
		return newResource.handle;
	}

	//--------------------------------------------------------------------------------------------------
	void FrameBufferCache::freeBuffer(FrameBuffer)
	{
		assert(false && "Not implemented");
	}

	//--------------------------------------------------------------------------------------------------
	void FrameBufferCache::freeTexture(Texture2d)
	{
		assert(false && "Not implemented");
	}

	//--------------------------------------------------------------------------------------------------
	void FrameBufferCache::freeResources()
	{
		for (auto& res : m_textures)
		{
			res.locked = false;
		}
		for (auto& res : m_frameBuffers)
		{
			res.locked = false;
		}
	}

	//--------------------------------------------------------------------------------------------------
	void FrameBufferCache::deallocateResources()
	{
		for (auto& res : m_textures)
		{
			m_device.destroyTexture2d(res.handle);
		}
		m_textures.clear();
		for (auto& res : m_frameBuffers)
		{
			//m_device.destroyFrameBuffer();
		}
		m_frameBuffers.clear();
	}
}
