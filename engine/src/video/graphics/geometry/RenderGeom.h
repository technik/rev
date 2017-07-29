//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#ifdef REV_USE_VULKAN
#include <vulkan/vulkan.h>
#endif // REV_USE_VULKAN

namespace rev {
	namespace video {

		// Basic element of geometry that can be used for rendering.
		// It is comprised of a set of interleaved buffers of data per vertex, plus a buffer of indices
		class RenderGeom {
		public:
			RenderGeom();
			uint32_t nVertices() const { return mNVerts; }
			uint32_t nIndices() const { return 3; }

#ifdef REV_USE_VULKAN
			vk::Buffer			mVertexBuffer;
			vk::DeviceMemory	mVertexBufferMemory;
			vk::Buffer			mIndexBuffer;
			vk::DeviceMemory	mIndexBufferMemory;
#endif // REV_USE_VULKAN

		private:
			uint32_t mNVerts;
		};
}}