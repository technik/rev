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

#include <gfx/renderer/RasterQueue.h>
#include <gfx/renderer/RasterHeap.h>
#include <math/algebra/matrix.h>

#include <vector>

namespace rev::gfx
{
	class GPUBuffer;

	class RasterScene : public RasterQueue
	{
	public:
		RasterScene();
		~RasterScene();

		void getDrawBatches(std::vector<Draw>& draws, std::vector<Batch>& batches) override;

		// Invalidates the order of renderables
		void addInstance(const math::Mat44f& worldMtx, uint32_t meshNdx);
		void clearInstances();

		gfx::RasterHeap m_geometry;

	private:
		void refreshMatrixBuffer();

		std::vector<uint32_t> m_instanceMeshNdx;
		std::vector<math::Mat44f> m_instanceWorldMtx;
		std::shared_ptr<GPUBuffer> m_worldMtxBuffer;
	};

	inline RasterScene::RasterScene()
	{}

	inline RasterScene::~RasterScene()
	{}
}
