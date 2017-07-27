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
		void RendererBackEnd::draw(const DrawBatch& _batch) {
			// Vertex buffer
			VkBuffer vertexBuffers[] = { (VkBuffer)_batch.vertexBuffer };
			VkDeviceSize offsets[] = {0}; // No offsets. We use interleaved vertex data, so there's only one buffer to bind
			vkCmdBindVertexBuffers(mCommandBuffer, 0, 1, vertexBuffers, offsets);
			// Index buffer
			vkCmdBindIndexBuffer(mCommandBuffer, (VkBuffer)_batch.indexBuffer, 0, VK_INDEX_TYPE_UINT16);
			// Draw
			for(auto drawCall : _batch.draws) {
				// Uniforms
				if(drawCall.uniformData.size > 0) {
					assert(drawCall.uniformData.data);
					vkCmdPushConstants(mCommandBuffer, mActivePipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, drawCall.uniformData.size, drawCall.uniformData.data);
				}
				//vkCmdBindDescriptorSets(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mActivePipelineLayout, 0, 1, &drawCall.mDescriptorSet, 0, nullptr);
				// Draw indices
				vkCmdDrawIndexed(mCommandBuffer, drawCall.nIndices, 1, drawCall.indexOffset, drawCall.vertexOffset, 0);
			}
		}

#endif // REV_USE_VULKAN
#ifdef OPENGL_45
		//------------------------------------------------------------------------------------------------------------------
		void RendererBackEnd::draw(const DrawCall& _dc) {
#ifndef REV_USE_VULKAN
			const DrawInfo& info = _dc.renderStateInfo;
			// Set shader
			mDriver->setShader(info.program);

			// Set global uniforms
			int uWorldViewProj = mDriver->getUniformLocation("uWorldViewProj");
			mDriver->setUniform(uWorldViewProj, info.wvp);

			int uLightClr = mDriver->getUniformLocation("uLightClr");
			mDriver->setUniform(uLightClr, info.lightClr);

			int uLightDir = mDriver->getUniformLocation("uLightDir");
			mDriver->setUniform(uLightDir, info.lightDir);

			int uViewPos = mDriver->getUniformLocation("uViewPos");
			mDriver->setUniform(uViewPos, info.viewPos);

			GLenum stage = 0;
			for (const auto& map : info.texUniforms) {
				int uLoc = mDriver->getUniformLocation(map.first.c_str());
				if (uViewPos >= 0) {
					mDriver->setUniform(uLoc, map.second, stage);
					++stage;
				}
			}

			for (const auto& v : info.vec3Uniforms) {
				int uLoc = mDriver->getUniformLocation(v.first.c_str());
				if (uViewPos >= 0) {
					mDriver->setUniform(uLoc, v.second);
				}
			}

			for (const auto& f : info.floatUniforms) {
				int uLoc = mDriver->getUniformLocation(f.first.c_str());
				if (uViewPos >= 0) {
					mDriver->setUniform(uLoc, f.second);
				}
			}

			// Actual draw
			StaticRenderMesh* mesh = _dc.mesh;
			glBindVertexArray(mesh->vao);
			glDrawElements(GL_TRIANGLES, mesh->nIndices, GL_UNSIGNED_SHORT, mesh->indices);
#endif // !REV_USE_VULKAN
	}

		//------------------------------------------------------------------------------------------------------------------
		void RendererBackEnd::drawSkybox(const DrawInfo& _info) {
#ifndef REV_USE_VULKAN
			if (!mSkyBox) {
				mSkyBox = game::Procedural::box(Vec3f(100.f));
}
#endif // !REV_USE_VULKAN
			DrawCall dc;
			dc.renderStateInfo = _info;
			dc.mesh = mSkyBox;
			glDisable(GL_CULL_FACE);
			draw(dc);
			glEnable(GL_CULL_FACE);
		}

		//------------------------------------------------------------------------------------------------------------------
		void RendererBackEnd::flush() {
			//glBindVertexArray(0);
		}
#endif // OPENGL_45
	}
}
