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
			NativeFrameBufferVulkan(const Window&, VkInstance _apiInstance, const VulkanDriver&);
#endif
			~NativeFrameBufferVulkan();

			const auto& imageViews() const { return mSwapChainImageViews; }
			VkSwapchainKHR swapChain() const { return mSwapChain; }
			VkFormat imageFormat() const { return mImageFormat; }
			bool sRGBColorSpace() const { return VK_COLOR_SPACE_SRGB_NONLINEAR_KHR == mColorSpace; }

		private:
#ifdef ANDROID
			bool initSurface(VkInstance _apiInstance);
#else
			bool initSurface(const Window& _wnd, const VulkanDriver& _driver);
#endif
			VkFormat		mImageFormat;
			VkInstance		mApiInstance;
			VkDevice		mDevice;

			VkSurfaceKHR	mSurface;
			VkSwapchainKHR	mSwapChain;
			VkColorSpaceKHR	mColorSpace;

			std::vector<VkImage>		mSwapChainImages;
			std::vector<VkImageView>	mSwapChainImageViews;
		};
	}
}
