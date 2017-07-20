//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <vulkan/vulkan.h>

namespace rev {
	namespace video {

		class Window;

		class NativeFrameBufferVulkan {
		public:
#ifdef ANDROID
			NativeFrameBufferVulkan();
#else
			NativeFrameBufferVulkan(Window*, VkInstance _apiInstance);
#endif
			~NativeFrameBufferVulkan();

		private:
#ifdef ANDROID
			bool initSurface(VkInstance _apiInstance);
#else
			bool initSurface(Window* _wnd, VkInstance _apiInstance);
#endif
			VkSurfaceKHR mSurface;
			VkInstance mApiInstance;
		};
	}
}
