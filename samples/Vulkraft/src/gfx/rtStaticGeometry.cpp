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
#include "rtStaticGeometry.h"
#include "bvh.h"

namespace vkft {

	namespace {
		uint16_t octahedronIndices[8*3] = 
		{
			0,1,2,
			0,2,3,
			0,3,4,
			0,4,1,
			2,1,5,
			3,2,5,
			4,3,5,
			1,4,5
		};

		rev::math::Vec3f octahedronVertices[6] = 
		{
			{ 0.0, 0.0, 0.0 },
			{ 0.5, 0.5, 0.0 },
			{ 0.0, 0.5, 0.5 },
			{ -0.5, 0.5, 0.0 },
			{ 0.0, 0.5,-0.5 },
			{ 0.0, 1.0, 0.0 }
		};
	}

	//--------------------------------------------------------------------------------------------------
	RTStaticGeometry::RTStaticGeometry(rev::gfx::Device& gfxDevice, rev::math::Vec3f* vtxBuffer, size_t nVertices, uint16_t* ndxBuffer, size_t nIndices) {
		// Build bvh
		std::vector<uint16_t> indices(ndxBuffer, ndxBuffer+nIndices);
		std::vector<rev::math::Vec3f> vertices(vtxBuffer, vtxBuffer+nVertices);
		// Sort triangles by centroid
		//AABBTree bvh(indices, vertices);

		// Create storage buffers

		// Submit buffers
			// Temporary, copy index buffers to different sized buffer
		uint32_t doubleIndices[8*3];
		for(int i = 0; i < 24; ++i)
		{
			doubleIndices[i] = ndxBuffer[i];
		}
		m_gpuIndices = gfxDevice.allocateStorageBuffer(sizeof(uint32_t)*nIndices, doubleIndices);
		m_gpuVertices = gfxDevice.allocateStorageBuffer(sizeof(rev::math::Vec3f)*nVertices, vtxBuffer);
	}

	//--------------------------------------------------------------------------------------------------
	RTStaticGeometry* RTStaticGeometry::loadGltf(rev::gfx::Device& gfxDevice, const char* fileName) {
		
		return new RTStaticGeometry(gfxDevice, octahedronVertices, 6, &octahedronIndices[0], 8*3);
	}
}