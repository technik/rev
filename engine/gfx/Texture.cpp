//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2021 Carmelo J Fdez-Aguera
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
#include "Texture.h"
#include "Image.h"

#include <core/tools/log.h>

namespace rev::gfx
{
	std::shared_ptr<Texture> Texture::loadFromMemory(
		const char* fileName,
		vk::SamplerAddressMode wrapS,
		vk::SamplerAddressMode wrapT,
		bool generateMipmaps,
		bool hdr,
		bool sRGB)
	{
		assert(!generateMipmaps && "Mipmap generation not yet supported");
		std::shared_ptr<Image<>> image = Image<>::load(fileName, 3, hdr, sRGB);

		auto texture = RenderContext().allocator().createTexture(
			fileName,
			image->size(),
			image->format(),
			wrapS,
			wrapT,
			false, // No anisotropy
			0,
			image->data(),
			vk::ImageUsageFlagBits::eSampled,
			RenderContext().graphicsQueueFamily()
		);

		if (!texture)
		{
			core::LogMessage(std::string("Unable to load texture ") + fileName);
		}

		return texture;
	}
}