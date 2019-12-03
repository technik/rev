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
#pragma once

#include <cstddef>
#include <memory>
#include <vector>
#include "textureSampler.h"
#include <math/algebra/vector.h>
#include "namedResource.h"
#include <graphics/Image.h>

namespace rev :: gfx
{
	struct Texture2d : NamedResource
	{
	public:

		struct Descriptor
		{
			Image::PixelFormat pixelFormat;
			TextureSampler sampler;
			bool sRGB = false;
			bool depth = false;
			math::Vec2u size;

			/// Number of faces in the texture. Usually 1 for regular textures, 6 for a cubemap.
			/// Can also be an arbitrary number for array textures.
			unsigned nFaces = 1;
			
			/// Number of mipmap levels to be used in this texture
			/// 0 means compute automatically
			size_t mipLevels = 0;

			/// Array of images to be used for mipmaps, or null if images will be initialized by the gpu
			/// If not null, the array must contain exactly "providedImages" images. Mip levels will be generated
			/// Automatically for the range [providedImages, mipLevels-1].
			/// If both multiple faces and multiple levels are stored, then all faces of the highest level
			/// are referenced first, then all faces of the second highest, and so on.
			std::vector<std::shared_ptr<Image>> srcImages;
		};

		Texture2d() = default;
		Texture2d(int32_t id) : NamedResource(id) {}

		bool operator==(const Texture2d& other) const
		{
			return m_id == other.m_id;
		}
	};
}
