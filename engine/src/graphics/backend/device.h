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
#include "renderQueue.h"
#include "texture2d.h"
#include "textureSampler.h"

namespace rev :: gfx
{
	class Device
	{
	public:
		virtual RenderQueue& renderQueue() = 0;

		// --- Stuff allocation ---
		// Texture sampler
		virtual TextureSampler createTextureSampler(const TextureSampler::Descriptor&) = 0;
		virtual void destroyTextureSampler(TextureSampler) = 0;

		// Texture
		virtual Texture2d createTexture2d(const Texture2d::Descriptor&) = 0;
		virtual void destroyTexture2d(TextureSampler) = 0;

	protected:
		Device() = default;
	};
}
