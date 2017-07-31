//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include "vulkanDriver.h"
#include <vulkan/vulkan.h>
#include <math/algebra/vector.h>

namespace rev {
	namespace video {

		class Window;

		class NativeFrameBufferVulkan {
		public:
			NativeFrameBufferVulkan(const Window&, VkInstance _apiInstance);
			~NativeFrameBufferVulkan();

			const math::Vec2u&	size			() const { return mSize; }
			bool				sRGBColorSpace	() const { return VK_COLOR_SPACE_SRGB_NONLINEAR_KHR == mColorSpace; }


			void begin();
			void end() {}


			//const auto& imageViews() const { return mSwapChainImageViews; }
			//VkSwapchainKHR swapChain() const { return mSwapChain; }
			//VkFormat imageFormat() const { return mImageFormat; }
			// Returns a description to use this framebuffer as an attachment inside a render pass

			// Vulkan specific code
			const VkAttachmentDescription&	attachmentDescription	() const { return mAttachDesc; }
			VkFramebuffer					activeBuffer			() const { return mSwapChainBuffers[mCurFBImageIndex]; }
			VkSemaphore						imageAvailable			() const { return mImageAvailableSemaphore; }
			void							setRenderPass			(VkRenderPass _pass) { createFrameBuffers(_pass); }

		private:
			bool initSurface(const Window& _wnd);
			void setupAttachmentDesc();
			bool createFrameBuffers(VkRenderPass _pass);

			VkFormat		mImageFormat;
			VkInstance		mApiInstance;
			VkDevice		mDevice;

			VkSurfaceKHR	mSurface;
			VkSwapchainKHR	mSwapChain;
			VkColorSpaceKHR	mColorSpace;


			uint32_t mCurFBImageIndex;

			math::Vec2u	mSize;

			// Descriptor to use this FB as attachment inside a render pass
			VkAttachmentDescription mAttachDesc = {}; // Initially clear

			std::vector<VkImage>		mSwapChainImages;
			std::vector<VkImageView>	mSwapChainImageViews;
			std::vector<VkFramebuffer>	mSwapChainBuffers;

			VkSemaphore mImageAvailableSemaphore;
		};
	}
}
