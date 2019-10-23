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
#include "load.h"
#include <graphics/backend/device.h>
#include <graphics/Image.h>
#include <core/platform/fileSystem/file.h>

namespace rev::game
{
	gfx::Texture2d load2dTextureFromFile(gfx::Device& device, gfx::TextureSampler sampler, std::string_view fileName, bool sRGB, int nChannels, int nMips)
	{
		// Load image from file
		auto image = gfx::Image::load(fileName, nChannels);
		if(!image)
			return gfx::Texture2d();

		// Fill out descriptor
		gfx::Texture2d::Descriptor descriptor;
		descriptor.pixelFormat = image->format();
		descriptor.sampler = sampler;
		descriptor.sRGB = sRGB;
		descriptor.size = image->size();
		descriptor.srcImages.emplace_back(std::move(image));
		descriptor.mipLevels = nMips;

		// Create texture in device
		return device.createTexture2d(descriptor);
	}
}