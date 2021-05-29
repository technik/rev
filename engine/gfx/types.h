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
#pragma once

#include <string>

#include <core/tools/log.h>
#include <gfx/backend/Vulkan/renderContextVulkan.h>

namespace rev::gfx {

	static size_t GetPixelSize(vk::Format fmt)
	{
		switch (fmt)
		{
		case vk::Format::eR32G32B32A32Sfloat:
			return sizeof(math::Vec4f);
		case vk::Format::eR32G32B32Sfloat:
			return sizeof(math::Vec3f);
		case vk::Format::eR32G32Sfloat:
			return sizeof(math::Vec2f);
		case vk::Format::eR8G8B8A8Srgb:
		case vk::Format::eR8G8B8A8Unorm:
			return sizeof(uint8_t) * 4;
		case vk::Format::eR8G8B8Srgb:
		case vk::Format::eR8G8B8Unorm:
			return sizeof(uint8_t) * 3;
		default:
			core::CheckError(false, "Unsupported texture format");
			return 0;
		}
	}

}