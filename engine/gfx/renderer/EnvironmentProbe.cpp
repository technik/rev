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
#include "EnvironmentProbe.h"
#include <gfx/backend/Vulkan/vulkanAllocator.h>
#include <gfx/backend/Vulkan/renderContextVulkan.h>

namespace rev::gfx
{
	std::shared_ptr<EnvironmentProbe> EnvironmentProbe::LoadProbe(std::string_view imageName, uint32_t numLevels, uint32_t maxResolution)
	{
		// Load image file
		// For now, just load the image and sample from it using spherical lat-long unwrap
		auto texture = Texture::loadFromMemory(imageName.data(), vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eClampToEdge, false, true, false);
		 
		// TODO:
		// Run compute pipeline on it
		// Copy SH back from it?
		// Compose the texture
		return std::shared_ptr<EnvironmentProbe>(new EnvironmentProbe(texture, 1, math::Vec4f::zero()));
	}
}