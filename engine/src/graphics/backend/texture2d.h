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
#include "textureSampler.h"
#include <math/algebra/vector.h>

namespace rev :: graphics {
	class Image;
}

namespace rev :: gfx
{
	class Texture2d
	{
	public:
		struct Descriptor
		{
			enum class ChannelFormat : GLenum
			{
				Byte = GL_UNSIGNED_BYTE,
				Float32 = GL_FLOAT
			} channelFormat;

			enum class PixelFormat : GLenum
			{
				RGB = GL_RGB,
				sRGB = GL_SRGB,
				sRGBA = GL_SRGB_ALPHA,
				RGBA = GL_RGBA
			} pixelFormat;

			TextureSampler sampler;
			math::Vec2u size;
			
			size_t mipLevels; ///< Number of mipmap levels to be used in this texture

			/// Chain of images to be used for mipmaps, or null if images will be initialized by the gpu
			/// If not null, the array must contain exactly "mipLevel" images. Mip levels will be generated
			/// Automatically starting on the first null pointer contained in the array, up to mipLevels-1.
			graphics::Image** srcImages;
		};

		int32_t id;

		static constexpr int32_t InvalidId = -1;
	};
}
