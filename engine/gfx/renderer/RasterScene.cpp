
#include <gfx/renderer/RasterScene.h>

namespace rev::gfx
{
	void RasterScene::getDrawBatches(std::vector<Draw>& draws, std::vector<Batch>& batches)
	{
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
		batch.endDraw = draws.size();
		batch.indexType = vk::IndexType::eUint32;
		batch.indexBuffer = m_geometry.indexBuffer();
		m_geometry.getVertexBindings(
			batch.positionBinding,
			batch.normalsBinding,
			batch.tangentsBinding,
			batch.texCoordBinding
		);

		// Update per instance model matrices if there has been any change
		auto mtxDst = m_ctxt->allocator().mapBuffer<math::Mat44f>(*m_mtxBuffers[m_doubleBufferNdx]);
		scene.m_sceneInstances.updatePoses(mtxDst);
		m_ctxt->allocator().unmapBuffer(mtxDst);

		auto& alloc = ctxt.allocator();
		m_mtxBuffers[0] = alloc.createBufferForMapping(sizeof(math::Mat44f) * maxNumInstances, vk::BufferUsageFlagBits::eStorageBuffer, m_ctxt->graphicsQueueFamily());
		m_mtxBuffers[1] = alloc.createBufferForMapping(sizeof(math::Mat44f) * maxNumInstances, vk::BufferUsageFlagBits::eStorageBuffer, m_ctxt->graphicsQueueFamily());

		// Allocate materials buffer
		size_t streamToken = 0;
		if (!scene.m_materials.empty())
		{
			m_materialsBuffer = alloc.createGpuBuffer(
				sizeof(gfx::PBRMaterial) * maxNumMaterials,
				vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst,
				m_ctxt->graphicsQueueFamily());
			streamToken = alloc.asyncTransfer(*m_materialsBuffer, scene.m_materials.data(), scene.m_materials.size());
		}
	}

	void RasterScene::addInstance(const math::Mat44f& worldMtx, size_t meshNdx)
	{
		m_instanceWorldMtx.push_back(worldMtx);
		m_instanceMeshNdx.push_back(meshNdx);
	}

	void RasterScene::clearInstances()
	{
		m_instanceWorldMtx.clear();
		m_instanceMeshNdx.clear();
	}
}
