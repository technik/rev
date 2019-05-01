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
#include "EnvironmentProbe.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <graphics/Image.h>
#include <memory>
#include <vector>
#include <core/string_util.h>
#include <graphics/backend/device.h>
#include <iostream>

using Json = nlohmann::json;

namespace rev::gfx
{
	//------------------------------------------------------------------------------------------------------------------
	/*EnvironmentProbe::EnvironmentProbe(Device& device, const std::string& jsonName)
	{
		auto folder = core::getPathFolder(jsonName);
		// Open json descriptor
		Json probeDesc;
		std::ifstream(jsonName) >> probeDesc;

		// Load all images
		m_numLevels = probeDesc.size();
		
		std::vector<Image> mipImages;
		for(size_t i = 0; i < m_numLevels; ++i)
		{
			auto& desc = probeDesc[i];
			auto image = Image::load(folder + std::string(desc["name"]), 4);
			if(!image.data<>())
			{
				std::cout << "EnvironmentProbe error: unable to load image " << folder << std::string(desc["name"]);
				m_numLevels = 0;
				return;
			}
			mipImages.push_back(image);
		}

		// Create a new texture with all the levels
		TextureSampler::Descriptor samplerDesc;
		samplerDesc.filter = TextureSampler::MinFilter::Trilinear;
		samplerDesc.wrapS = TextureSampler::Wrap::Repeat;
		samplerDesc.wrapT = TextureSampler::Wrap::Clamp;
		//auto sampler = device.createTextureSampler(samplerDesc);

		Texture2d::Descriptor descriptor;
		//descriptor.sampler = sampler;
		descriptor.pixelFormat = mipImages[0].format();
		descriptor.mipLevels = mipImages.size();
		descriptor.providedImages = mipImages.size();
		descriptor.srcImages = mipImages.data();
		descriptor.size = mipImages[0].size();
		descriptor.sRGB = true;
		//m_texture = device.createTexture2d(descriptor);
	}*/
}