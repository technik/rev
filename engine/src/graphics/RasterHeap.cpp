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
#include "RasterHeap.h"

namespace rev::gfx {

	size_t RasterHeap::addPrimitiveData(
		uint32_t numVertices,
		const math::Vec3f* vtxPos,
		const math::Vec3f* normals,
		const math::Vec2f* uvs,
		uint32_t numIndices,
		const uint32_t* indices)
	{
		// Allocate space for the new vertices and indices
		const size_t numVerticesBefore = m_vtxPositions.size();
		m_vtxPositions.resize(numVerticesBefore + numVertices);
		m_vtxNormals.resize(numVerticesBefore + numVertices);
		m_textureCoords.resize(numVerticesBefore + numVertices);

		const size_t numIndicesBefore = m_indices.size();
		m_indices.resize(numIndicesBefore + numIndices);

		// Copy primitive data
		memcpy(&m_vtxPositions[numVerticesBefore], vtxPos, sizeof(math::Vec3f) * numVertices);
		memcpy(&m_vtxNormals[numVerticesBefore], normals, sizeof(math::Vec3f) * numVertices);
		memcpy(&m_textureCoords[numVerticesBefore], uvs, sizeof(math::Vec2f) * numVertices);
		memcpy(&m_indices[numIndicesBefore], indices, sizeof(uint32_t) * numIndices);

		auto& primitive = m_primitives.emplace_back();
		// Ensure mesh data is within the limits the renderer can handle.
		// This ensures the casts to uint32_t below are safe.
		assert(m_vtxPositions.size() < std::numeric_limits<uint32_t>::max());
		assert(m_indices.size() < std::numeric_limits<uint32_t>::max());

		primitive.vtxOffset = (uint32_t)numVerticesBefore;
		primitive.indexOffset = (uint32_t)numIndicesBefore;
		primitive.numIndices = numIndices;

		return m_primitives.size() - 1;
	}

}