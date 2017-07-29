//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <vulkan/vulkan.h>
#include "vulkanDriver.h"
#include <math/algebra/vector.h>

namespace rev {
	namespace video {

		class Window;

		class NativeFrameBufferVulkan {
		public:
			NativeFrameBufferVulkan(const Window&, VkInstance _apiInstance);
			~NativeFrameBufferVulkan();

			const math::Vec2u& size() const { return mSize; }
			const auto& imageViews() const { return mSwapChainImageViews; }
			VkSwapchainKHR swapChain() const { return mSwapChain; }
			//VkFormat imageFormat() const { return mImageFormat; }
			bool sRGBColorSpace() const { return VK_COLOR_SPACE_SRGB_NONLINEAR_KHR == mColorSpace; }

			// Returns a description to use this framebuffer as an attachment inside a render pass
			const VkAttachmentDescription&	attachmentDescription() const { return mAttachDesc; }

		private:
#ifdef ANDROID
			bool initSurface(VkInstance _apiInstance);
#else
			bool initSurface(const Window& _wnd);
#endif
			void setupAttachmentDesc();

			VkFormat		mImageFormat;
			VkInstance		mApiInstance;
			VkDevice		mDevice;

			VkSurfaceKHR	mSurface;
			VkSwapchainKHR	mSwapChain;
			VkColorSpaceKHR	mColorSpace;

			math::Vec2u	mSize;

			// Descriptor to use this FB as attachment inside a render pass
			VkAttachmentDescription mAttachDesc = {}; // Initially clear

			std::vector<VkImage>		mSwapChainImages;
			std::vector<VkImageView>	mSwapChainImageViews;
		};
	}
}
