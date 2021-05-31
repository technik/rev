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
#pragma once

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include <gfx/backend/Vulkan/Vulkan.h>

namespace rev::gfx
{
	class GPUBuffer;

	// Utility to create a bunch of
	class RasterQueue
	{
	public:
		struct Draw
		{
			uint32_t numIndices;
			uint32_t indexOffset;
			uint32_t vtxOffset;
			uint32_t numInstances;
			uint32_t instanceOffset;
			uint32_t materialIndex;
		};

		using VtxBinding = std::pair<GPUBuffer*, uint32_t>;

		struct Batch
		{
			uint32_t firstDraw;
			uint32_t endDraw;

			vk::IndexType indexType;

			GPUBuffer* indexBuffer;
			VtxBinding positionBinding;
			VtxBinding normalsBinding;
			VtxBinding tangentsBinding;
			VtxBinding texCoordBinding;

			std::shared_ptr<GPUBuffer> worldMatrices;
			std::shared_ptr<GPUBuffer> materials;
			std::shared_ptr<GPUBuffer> textures;
		};

		virtual void getDrawBatches(std::vector<Draw>& draws, std::vector<Batch>& batches) = 0;
	};
}