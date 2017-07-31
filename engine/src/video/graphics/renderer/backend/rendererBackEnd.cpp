//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/07
//----------------------------------------------------------------------------------------------------------------------
// Render system's back end, the layer right on top of the video driver
#include "rendererBackEnd.h"

#ifdef REV_USE_VULKAN

#endif // REV_USE_VULKAN
#ifdef OPENGL_45

#include <math/algebra/matrix.h>
#include <video/graphics/driver/graphicsDriver.h>
#include <video/graphics/staticRenderMesh.h>

using namespace rev::math;
#endif // OPENGL_45

namespace rev {
	namespace video {
#ifdef REV_USE_VULKAN
		//------------------------------------------------------------------------------------------------------------------
		RendererBackEnd::~RendererBackEnd() {
		}

		//------------------------------------------------------------------------------------------------------------------
		void RendererBackEnd::draw(const DrawBatch& _batch) {
			// Vertex buffer
			VkBuffer vertexBuffers[] = { (VkBuffer)_batch.vertexBuffer };
			VkDeviceSize offsets[] = {0}; // No offsets. We use interleaved vertex data, so there's only one buffer to bind
			vkCmdBindVertexBuffers(mTargetCmdBuffer, 0, 1, vertexBuffers, offsets);
			// Index buffer
			vkCmdBindIndexBuffer(mTargetCmdBuffer, (VkBuffer)_batch.indexBuffer, 0, VK_INDEX_TYPE_UINT16);
			// Draw
			for(auto drawCall : _batch.draws) {
				// Uniforms
				if(drawCall.uniformData.size > 0) {
					assert(drawCall.uniformData.data);
					vkCmdPushConstants(mTargetCmdBuffer, mActivePipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, drawCall.uniformData.size, drawCall.uniformData.data);
				}
				//vkCmdBindDescriptorSets(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mActivePipelineLayout, 0, 1, &drawCall.mDescriptorSet, 0, nullptr);
				// Draw indices
				vkCmdDrawIndexed(mTargetCmdBuffer, drawCall.nIndices, 1, drawCall.indexOffset, drawCall.vertexOffset, 0);
			}
		}

#endif // REV_USE_VULKAN
	}
}
