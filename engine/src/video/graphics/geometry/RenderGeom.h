//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#ifdef REV_USE_VULKAN
#include <vulkan/vulkan.hpp>
#endif // REV_USE_VULKAN

namespace rev {
	namespace video {

		// Basic element of geometry that can be used for rendering.
		class RenderGeom {
		public:
			RenderGeom();
			uint32_t nVertices() const { return mNVerts; }

#ifdef REV_USE_VULKAN
			vk::Buffer			mVertexBuffer;
			vk::DeviceMemory	mVertexBufferMemory;
#endif // REV_USE_VULKAN

		private:
			uint32_t mNVerts;
		};
}}