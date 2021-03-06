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

#include <gfx/backend/Vulkan/gpuBuffer.h>
#include <gfx/backend/Vulkan/renderContextVulkan.h>
#include <gfx/backend/Vulkan/vulkanAllocator.h>
#include <math/geometry/mesh.h>

using namespace rev::math;

namespace rev::gfx {

	// Out of line constructor to enable use of shared_ptr with just a forward reference
	RasterHeap::~RasterHeap()
	{}

	size_t RasterHeap::addPrimitiveData(
		uint32_t numVertices,
		const Vec3f* vtxPos,
		const Vec3f* normals,
		const Vec4f* tangents,
		const Vec2f* uvs,
		uint32_t numIndices,
		const uint32_t* indices,
		uint32_t materialNdx)
	{
		assert(!isClosed());

		std::vector<Vec3f> recomputedNormals;
		if (normals == nullptr)
		{
			recomputedNormals = generateNormals(numVertices, vtxPos, numIndices, indices);
			normals = recomputedNormals.data();
		}

		std::vector<Vec4f> recomputedTangents;
		if (tangents == nullptr)
		{
			recomputedTangents = generateTangentSpace(numVertices, vtxPos, uvs, normals, numIndices, indices);
			tangents = recomputedTangents.data();
		}

		// Allocate space for the new vertices and indices
		const size_t numVerticesBefore = m_vtxPositions.size();
		m_vtxPositions.resize(numVerticesBefore + numVertices);
		m_vtxNormals.resize(numVerticesBefore + numVertices);
		m_vtxTangents.resize(numVerticesBefore + numVertices);
		m_textureCoords.resize(numVerticesBefore + numVertices);

		const size_t numIndicesBefore = m_indices.size();
		m_indices.resize(numIndicesBefore + numIndices);

		// Copy primitive data
		memcpy(&m_vtxPositions[numVerticesBefore], vtxPos, sizeof(Vec3f) * numVertices);
		memcpy(&m_vtxNormals[numVerticesBefore], normals, sizeof(Vec3f) * numVertices);
		memcpy(&m_vtxTangents[numVerticesBefore], tangents, sizeof(Vec4f) * numVertices);
		memcpy(&m_textureCoords[numVerticesBefore], uvs, sizeof(Vec2f) * numVertices);
		memcpy(&m_indices[numIndicesBefore], indices, sizeof(uint32_t) * numIndices);

		auto& primitive = m_primitives.emplace_back();
		// Ensure mesh data is within the limits the renderer can handle.
		// This ensures the casts to uint32_t below are safe.
		assert(m_vtxPositions.size() < std::numeric_limits<uint32_t>::max());
		assert(m_indices.size() < std::numeric_limits<uint32_t>::max());

		primitive.vtxOffset = (uint32_t)numVerticesBefore;
		primitive.indexOffset = (uint32_t)numIndicesBefore;
		primitive.numIndices = numIndices;
		primitive.materialNdx = materialNdx;

		return m_primitives.size() - 1;
	}

	size_t RasterHeap::closeAndSubmit(
		RenderContextVulkan& renderContext,
		VulkanAllocator& alloc)
	{
		assert(!isClosed());

		const auto numVertices = m_vtxPositions.size();
		const auto vtxDataSize = numVertices * (sizeof(Vec3f) * 2 + sizeof(Vec4f) + sizeof(Vec2f)); // Pos+Normal + UVs
		const auto indexDataSize = m_indices.size() * sizeof(uint32_t);

		m_vtxBuffer = alloc.createGpuBuffer(
			vtxDataSize,
			vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
			renderContext.graphicsQueueFamily());

		m_indexBuffer = alloc.createGpuBuffer(
			indexDataSize,
			vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
			renderContext.graphicsQueueFamily());

		// Stream it into GPU memory
		alloc.reserveStreamingBuffer(vtxDataSize + indexDataSize);

		m_vtxPosOffset = 0;
		alloc.asyncTransfer(*m_vtxBuffer, m_vtxPositions.data(), numVertices, m_vtxPosOffset);
		m_normalsOffset = uint32_t(numVertices * sizeof(Vec3f));
		alloc.asyncTransfer(*m_vtxBuffer, m_vtxNormals.data(), numVertices, m_normalsOffset);
		m_tangentsOffset = uint32_t(numVertices * sizeof(Vec3f)) + m_normalsOffset;
		alloc.asyncTransfer(*m_vtxBuffer, m_vtxTangents.data(), numVertices, m_tangentsOffset);
		m_texCoordOffset = uint32_t(numVertices * sizeof(Vec4f)) + m_tangentsOffset;
		alloc.asyncTransfer(*m_vtxBuffer, m_textureCoords.data(), numVertices, m_texCoordOffset);

		auto streamToken = alloc.asyncTransfer(*m_indexBuffer, m_indices.data(), m_indices.size(), 0);

		// Materials
		if (!m_materials.empty())
		{
			m_materialsBuffer = alloc.createGpuBuffer(
				sizeof(gfx::PBRMaterial) * m_materials.size(),
				vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst,
				RenderContext().graphicsQueueFamily());
			streamToken = alloc.asyncTransfer(*m_materialsBuffer, m_materials.data(), m_materials.size());
		}

		// Get rid of local data
		m_vtxPositions.clear();
		m_vtxNormals.clear();
		m_vtxTangents.clear();
		m_textureCoords.clear();
		m_indices.clear();
		m_materials.clear();

		return streamToken;
	}

	void RasterHeap::getVertexBindings(VtxBinding& pos, VtxBinding& normal, VtxBinding& tangent, VtxBinding& uvs)
	{
		pos = { m_vtxBuffer.get(), m_vtxPosOffset };
		normal = { m_vtxBuffer.get(), m_normalsOffset };
		tangent = { m_vtxBuffer.get(), m_tangentsOffset };
		uvs = { m_vtxBuffer.get(), m_texCoordOffset };
	}

	bool RasterHeap::isClosed() const
	{
		return m_vtxBuffer != nullptr;
	}
}