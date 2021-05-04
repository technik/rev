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

#include <cstdint>
#include <math/algebra/vector.h>

namespace fx::gltf {
	class Document;
}

namespace rev::gfx
{
	// Utility to create a bunch of
	class RasterHeap
	{
	public:
		struct alignas(32) Primitive
		{
			uint32_t vtxOffset;
			uint32_t indexOffset;
			uint32_t numIndices;
			uint32_t primitiveNdx;
			// uint32_t materialNdx;
		};

	public:
		~RasterHeap(); // Destroy heap and deallocate resources (Both CPU and GPU)

		// Disable copy
		RasterHeap(const RasterHeap&) = delete;
		RasterHeap& operator=(const RasterHeap&) = delete;

		// Add per primitive data (vtx, normal, ...) -> primitive id
		size_t addPrimitiveData(
			uint32_t numVertices,
			const math::Vec3f* vtxPos,
			const math::Vec3f* normals,
			const math::Vec2f* uvs,
			uint32_t numIndices,
			const uint32_t* indices
			);

		const Primitive& getPrimitiveById(size_t primitiveId);


		// Close the heap. After this point you can't add any more meshes to the heap, but 
		// you can submit it to the GPU for use in draw calls
		void close();
		// Submit GPU data
		void submitToGPU();

		// Load utilities
		void loadGltfMesh(const fx::gltf::Document&);

	private:
		// Vtx data
		// Normals data
		// ...
		// Primitives ?
		// Materials?
		// Matrices?
	};
}