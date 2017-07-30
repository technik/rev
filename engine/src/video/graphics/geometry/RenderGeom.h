//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#ifdef REV_USE_VULKAN
#include <vulkan/vulkan.h>
#endif // REV_USE_VULKAN

#include "VertexFormat.h"

namespace rev {
	namespace video {

		// Basic element of geometry that can be used for rendering.
		// It is comprised of a set of interleaved buffers of data per vertex, plus a buffer of indices
		class RenderGeom {
		public:
			RenderGeom(const VertexFormat&, uint32_t _nVertices, const void* _vtxData, uint32_t _nIndices, const uint16_t* _indices);
			uint32_t nVertices() const { return mNVertices; }
			uint32_t nIndices() const { return mNIndices; }

			const VertexFormat& vertexFormat() const { return mVtxFormat; }

#ifdef REV_USE_VULKAN
			void sendBuffersToGPU();
			VkBuffer		mVertexBuffer;
			VkDeviceMemory	mVertexBufferMemory;
			VkBuffer		mIndexBuffer;
			VkDeviceMemory	mIndexBufferMemory;
#endif // REV_USE_VULKAN

		private:
			VertexFormat mVtxFormat;

			uint32_t mNVertices;
			const void* mVertexData;
			uint32_t mNIndices;
			const uint16_t* mIndices;
		};
}}