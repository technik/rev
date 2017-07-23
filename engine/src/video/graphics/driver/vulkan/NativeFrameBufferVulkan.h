//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <vulkan/vulkan.hpp>
#include "vulkanDriver.h"
#include <math/algebra/vector.h>

namespace rev {
	namespace video {

		class Window;

		class NativeFrameBufferVulkan {
		public:
#ifdef ANDROID
			NativeFrameBufferVulkan();
#else
			NativeFrameBufferVulkan(const Window&, vk::Instance _apiInstance, const VulkanDriver&);
#endif
			~NativeFrameBufferVulkan();

			const math::Vec2u& size() const { return mSize; }
			const auto& imageViews() const { return mSwapChainImageViews; }
			vk::SwapchainKHR swapChain() const { return mSwapChain; }
			//VkFormat imageFormat() const { return mImageFormat; }
			bool sRGBColorSpace() const { return vk::ColorSpaceKHR::eSrgbNonlinear == mColorSpace; }

			// Returns a description to use this framebuffer as an attachment inside a render pass
			const VkAttachmentDescription&	attachmentDescription() const { return mAttachDesc; }

		private:
#ifdef ANDROID
			bool initSurface(VkInstance _apiInstance);
#else
			bool initSurface(const Window& _wnd, const VulkanDriver& _driver);
#endif
			void setupAttachmentDesc();
			vk::Format			mImageFormat;
			vk::Instance		mApiInstance;
			vk::Device			mDevice;

			vk::SurfaceKHR		mSurface;
			vk::SwapchainKHR	mSwapChain;
			vk::ColorSpaceKHR	mColorSpace;

			math::Vec2u	mSize;

			// Descriptor to use this FB as attachment inside a render pass
			vk::AttachmentDescription mAttachDesc = {}; // Initially clear

			std::vector<vk::Image>		mSwapChainImages;
			std::vector<vk::ImageView>	mSwapChainImageViews;
		};
	}
}
