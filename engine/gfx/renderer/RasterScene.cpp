
#include <gfx/renderer/RasterScene.h>
#include <gfx/backend/Vulkan/gpuBuffer.h>
#include <gfx/backend/Vulkan/renderContextVulkan.h>
#include <gfx/backend/Vulkan/vulkanAllocator.h>

namespace rev::gfx
{
	void RasterScene::getDrawBatches(std::vector<Draw>& draws, std::vector<Batch>& batches)
	{
		refreshMatrixBuffer();

		assert(draws.empty());
		// Count on having at least one primitive per mesh
		draws.reserve(m_instanceMeshNdx.size());

		for (uint32_t i = 0; i < m_instanceMeshNdx.size(); ++i)
		{
			const auto& mesh = m_geometry.mesh(i);

			Draw instanceDraw = {};
			instanceDraw.instanceOffset = i; // Used to look up the world matrix
			instanceDraw.numInstances = 1;
			
			for (uint32_t primitiveNdx = mesh.firstPrimitive; primitiveNdx != mesh.endPrimitive; ++primitiveNdx)
			{
				auto& primitive = m_geometry.getPrimitiveById(primitiveNdx);
				instanceDraw.indexOffset = primitive.indexOffset;
				instanceDraw.numIndices = primitive.numIndices;
				instanceDraw.materialIndex = primitive.materialNdx;
				instanceDraw.vtxOffset = primitive.vtxOffset;
			}

			draws.push_back(instanceDraw);
		}

		auto& batch = batches.emplace_back();
		batch.firstDraw = 0;
		batch.endDraw = (uint32_t)draws.size();
		batch.indexType = vk::IndexType::eUint32;
		batch.indexBuffer = m_geometry.indexBuffer();
		m_geometry.getVertexBindings(
			batch.positionBinding,
			batch.normalsBinding,
			batch.tangentsBinding,
			batch.texCoordBinding
		);
	}

	void RasterScene::addInstance(const math::Mat44f& worldMtx, uint32_t meshNdx)
	{
		m_instanceWorldMtx.push_back(worldMtx);
		m_instanceMeshNdx.push_back(meshNdx);
	}

	void RasterScene::clearInstances()
	{
		m_instanceWorldMtx.clear();
		m_instanceMeshNdx.clear();
		m_worldMtxBuffer = nullptr;
	}

	void RasterScene::refreshMatrixBuffer()
	{
		if (!m_worldMtxBuffer && !m_instanceWorldMtx.empty())
		{
			auto& rc = RenderContext();
			auto& alloc = rc.allocator();
			m_worldMtxBuffer = alloc.createBufferForMapping(
				sizeof(math::Mat44f) * m_instanceWorldMtx.size(),
				vk::BufferUsageFlagBits::eStorageBuffer,
				rc.graphicsQueueFamily());

			auto mtxDst = alloc.mapBuffer<math::Mat44f>(*m_worldMtxBuffer);
			memcpy(mtxDst, m_instanceWorldMtx.data(), sizeof(math::Mat44f) * m_instanceWorldMtx.size());
			alloc.unmapBuffer(mtxDst);
		}
	}
}
