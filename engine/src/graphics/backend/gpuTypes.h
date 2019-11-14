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
#include <cstddef>
#include <cstdint>
#include "texture2d.h"
#include "namedResource.h"

namespace rev::gfx {

	struct FrameBuffer : NamedResource
	{
	public:
		struct Attachment
		{
			enum ImageType
			{
				Texture,
				RenderBuffer
			} imageType = ImageType::Texture;

			enum Target
			{
				Color,
				Depth
			} target;

			// Info for texture attachments
			Texture2d texture;
			size_t mipLevel = 0;
			// TODO: Layers and cubemap sides

			// Info for render buffer attachments
			// TODO

			bool operator==(const Attachment& other) const
			{
				return imageType == other.imageType
					&& target == other.target
					&& texture == other.texture
					&& mipLevel == other.mipLevel;
			}
		};

		struct Descriptor {
			static constexpr size_t cMaxAttachments = 8;
			size_t numAttachments;
			Attachment attachments[cMaxAttachments];

			Descriptor() = default;
			Descriptor(size_t n, Attachment* value)
				: numAttachments(n)
			{
				for (size_t i = 0; i < n; ++i)
					attachments[i] = value[i];
			}

			bool operator==(const Descriptor& other) const
			{
				if (numAttachments != other.numAttachments)
					return false;
				for (size_t i = 0; i < numAttachments; ++i)
				{
					if (!(attachments[i] == other.attachments[i]))
					{
						return false;
					}
				}
				return true;
			}
		};

		FrameBuffer(int32_t id) : NamedResource(id) {}
		FrameBuffer() = default;
	};

}
