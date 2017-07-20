//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <vulkan/vulkan.h>
#include "vulkanDriver.h"

namespace rev {
	namespace video {

		class Window;

		class NativeFrameBufferVulkan {
		public:
#ifdef ANDROID
			NativeFrameBufferVulkan();
#else
			NativeFrameBufferVulkan(Window*, VkInstance _apiInstance, VulkanDriver*);
#endif
			~NativeFrameBufferVulkan();

			bool sRGBColorSpace() const { return VK_COLOR_SPACE_SRGB_NONLINEAR_KHR == mColorSpace; }

		private:
#ifdef ANDROID
			bool initSurface(VkInstance _apiInstance);
#else
			bool initSurface(Window* _wnd, VkInstance _apiInstance);
#endif
			VkInstance		mApiInstance;
			VkDevice		mDevice;

			VkSurfaceKHR	mSurface;
			VkSwapchainKHR	mSwapChain;
			VkColorSpaceKHR	mColorSpace;
		};
	}
}
