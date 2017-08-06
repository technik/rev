//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

//#include "vulkanDriver.h"
#include <vulkan/vulkan.h>
#include <math/algebra/vector.h>
#include <vector>

namespace rev {
	namespace video {

		class Window;

		class NativeFrameBufferVulkan {
		public:
			enum class ZBufferFormat {
				eF32,
				eF24,
				eNone
			};

		public:
			NativeFrameBufferVulkan(const Window&, VkInstance _apiInstance, ZBufferFormat = ZBufferFormat::eNone);
			~NativeFrameBufferVulkan();

			const math::Vec2u&	size			() const { return mSize; }
			float				aspectRatio		() const { return float(mSize.x)/mSize.y; }
			bool				sRGBColorSpace	() const { return VK_COLOR_SPACE_SRGB_NONLINEAR_KHR == mColorSpace; }

			void begin();
			void end(VkSemaphore _renderSemaphore);

			// Vulkan specific code
			const VkAttachmentDescription&	attachmentDescription	() const { return mAttachDesc; }
			const VkAttachmentDescription&	depthAttachmentDesc		() const { return mDepthAttachDesc; }
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
			VkAttachmentDescription mDepthAttachDesc = {}; // Initially clear

			std::vector<VkImage>		mSwapChainImages;
			std::vector<VkImageView>	mSwapChainImageViews;
			std::vector<VkFramebuffer>	mSwapChainBuffers;

			// ZBuffer
			VkImage			mDepthImage = VK_NULL_HANDLE;
			VkDeviceMemory	mDepthImageMemory = VK_NULL_HANDLE;
			VkImageView		mDepthImageView = VK_NULL_HANDLE;

			VkSemaphore mImageAvailableSemaphore;
		};
	}
}
