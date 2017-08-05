//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#ifdef ANDROID
#define VK_USE_PLATFORM_ANDROID_KHR 1
#include <dlfcn.h>
#endif
#include "NativeFrameBufferVulkan.h"
#include <video/window/window.h>
#include <iostream>
#include <vector>

#include <vulkan/vk_platform.h>
#include <core/log.h>


using namespace std;

namespace rev { namespace video {

	namespace {	// anonymous namespace for vulkan utilities

		//--------------------------------------------------------------------------------------------------------------
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
			assert(!availableFormats.empty());
			// When format is undefined, it means we get to choose whatever format we want
			if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
				return { VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
			}

			// Prefer rgba8 + sRGB
			for (const auto& availableFormat : availableFormats) {
				if (availableFormat.format == VK_FORMAT_R8G8B8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
					return availableFormat;
				}
			}

			// Prefer rgba8
			for (const auto& availableFormat : availableFormats) {
				if (availableFormat.format == VK_FORMAT_R8G8B8A8_UNORM) {
					return availableFormat;
				}
			}

			// Return whatever we have
			return availableFormats[0];
		}

		//--------------------------------------------------------------------------------------------------------------
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes) {
			return VK_PRESENT_MODE_FIFO_KHR;
		}
	}

	//--------------------------------------------------------------------------------------------------------------
	NativeFrameBufferVulkan::NativeFrameBufferVulkan(const Window& _wnd, VkInstance _apiInstance, ZBufferFormat _zFormat)
		: mApiInstance(_apiInstance)
		, mDevice(GraphicsDriver::get().device())
	{
		core::Log::debug(" ----- NativeFrameBufferVulkan::NativeFrameBufferVulkan -----");
		if(!initSurface(_wnd))
			return;

		// Swap chain options
		auto support = GraphicsDriver::get().querySwapChainSupport(mSurface);
		auto surfaceFormat = chooseSwapSurfaceFormat(support.formats);
		mImageFormat = surfaceFormat.format;
		auto presentMode = chooseSwapPresentMode(support.presentModes);
		auto bufferExtent = support.capabilities.currentExtent;
		mSize = { bufferExtent.width, bufferExtent.height };

		// Swap chain creation info
		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = mSurface;
		createInfo.minImageCount = 2;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = bufferExtent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		// Create the swap chain
		if(vkCreateSwapchainKHR(mDevice, &createInfo, nullptr, &mSwapChain) != VK_SUCCESS) {
			core::Log::error("failed to create swap chain!");
			return; 
		}

		// Retrieve buffer image handles
		uint32_t imageCount;
		vkGetSwapchainImagesKHR(mDevice, mSwapChain, &imageCount, nullptr);
		mSwapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(mDevice, mSwapChain, &imageCount, mSwapChainImages.data());

		// Create image views for the images
		mSwapChainImageViews.reserve(imageCount);
		for(auto image : mSwapChainImages) {
			VkImageView view = GraphicsDriver::get().createImageView(image, mImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
			if(view == VK_NULL_HANDLE) {
				core::Log::error("failed to create image views within the native frame buffer\n");
			}
			mSwapChainImageViews.push_back(view);
		}

		// Init ZBuffer if needed
		if(_zFormat != ZBufferFormat::eNone) {
			GraphicsDriver::get().createImage(mSize, VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mDepthImage, mDepthImageMemory);
			mDepthImageView = GraphicsDriver::get().createImageView(mDepthImage, VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT);

			VkCommandPool cmdPool = GraphicsDriver::get().createCommandPool(true);
			// Create command buffer
			VkCommandBufferAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandPool = cmdPool;
			allocInfo.commandBufferCount = 1;

			VkCommandBuffer commandBuffer;
			vkAllocateCommandBuffers(mDevice, &allocInfo, &commandBuffer);

			// Record cmd buffer
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(commandBuffer, &beginInfo);

			VkImageMemoryBarrier barrier = {};
			VkImageLayout oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			VkImageLayout newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.oldLayout = oldLayout;
			barrier.newLayout = newLayout;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.image = mDepthImage;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			barrier.subresourceRange.baseMipLevel = 0;
			barrier.subresourceRange.levelCount = 1;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = 1;
			if (oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
				barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			} else if (oldLayout == VK_IMAGE_LAYOUT_PREINITIALIZED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
				barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			} else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			} else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			}

			vkCmdPipelineBarrier(
				commandBuffer,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				0,
				0, nullptr,
				0, nullptr,
				1, &barrier
			);

			vkEndCommandBuffer(commandBuffer);

			// Submit command buffer
			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;

			vkQueueSubmit(GraphicsDriver::get().graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
			vkQueueWaitIdle(GraphicsDriver::get().graphicsQueue());
			// Destroy command pool
			vkDestroyCommandPool(mDevice, cmdPool, nullptr);
		}

		// Set up attachment desc so this FB can be used in a render pass
		setupAttachmentDesc(); // Depends on FB configuration being ready
		core::Log::debug(" ----- /NativeFrameBufferVulkan::NativeFrameBufferVulkan -----");

		// Semaphore for synchronization
		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mImageAvailableSemaphore);
	}

	//--------------------------------------------------------------------------------------------------------------
	NativeFrameBufferVulkan::~NativeFrameBufferVulkan() {
		// Destroy frame buffers
		for (size_t i = 0; i < mSwapChainBuffers.size(); i++) {
			vkDestroyFramebuffer(mDevice, mSwapChainBuffers[i], nullptr);
		}
		// Destroy image views
		for(auto view : mSwapChainImageViews)
			vkDestroyImageView(mDevice, view, nullptr);

		vkDestroySwapchainKHR(mDevice, mSwapChain, nullptr);
		vkDestroySurfaceKHR(mApiInstance, mSurface, nullptr);
		vkDestroySemaphore(mDevice, mImageAvailableSemaphore, nullptr);
	}

	//--------------------------------------------------------------------------------------------------------------
	void NativeFrameBufferVulkan::begin() {
		vkAcquireNextImageKHR(mDevice, mSwapChain, std::numeric_limits<uint64_t>::max(), mImageAvailableSemaphore, VK_NULL_HANDLE, &mCurFBImageIndex);
	}

	//--------------------------------------------------------------------------------------------------------------
	void NativeFrameBufferVulkan::end(VkSemaphore _renderSemaphore) {
		VkSemaphore renderSemaphores[] = { _renderSemaphore };

		// Present
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = renderSemaphores;

		VkSwapchainKHR swapChains[] = { mSwapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &mCurFBImageIndex;

		presentInfo.pResults = nullptr; // Optional
		vkQueuePresentKHR(GraphicsDriver::get().graphicsQueue(), &presentInfo);
	}

	//--------------------------------------------------------------------------------------------------------------
	bool NativeFrameBufferVulkan::initSurface(const Window& _wnd) {
#ifdef _WIN32
		VkWin32SurfaceCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = _wnd.winapiHandle();
		createInfo.hinstance = GetModuleHandle(nullptr);

		// Get the extension
		auto CreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR) vkGetInstanceProcAddr(mApiInstance, "vkCreateWin32SurfaceKHR");

		// Create the surface
		if (!CreateWin32SurfaceKHR || CreateWin32SurfaceKHR(mApiInstance, &createInfo, nullptr, &mSurface) != VK_SUCCESS) {
			cout << "failed to create window surface!\n";
			return false;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(GraphicsDriver::get().physicalDevice(), GraphicsDriver::get().graphicsFamily(), mSurface, &presentSupport);

		return presentSupport;
#endif // _WIN32
#ifdef ANDROID
		core::Log::debug(" ----- VulkanDriver::initSurface -----");
		VkAndroidSurfaceCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
		createInfo.window = _wnd.nativeWindow;

		void* vulkan_so = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
			if (!vulkan_so) {
				core::Log::error("Vulkan lib not found");
				return false;
			}

		PFN_vkGetInstanceProcAddr getInstanceProcAddr =
			reinterpret_cast<PFN_vkGetInstanceProcAddr>(
				dlsym(vulkan_so, "vkGetInstanceProcAddr"));

		if(!getInstanceProcAddr) {
			core::Log::error("Unable to get vkGetInstanceProcAddr");
			return false;
		}

		PFN_vkCreateAndroidSurfaceKHR CreateAndroidSurfaceKHR = (PFN_vkCreateAndroidSurfaceKHR) vkGetInstanceProcAddr(mApiInstance, "vkCreateAndroidSurfaceKHR");
		if(!CreateAndroidSurfaceKHR) {
			core::Log::error("Unable to get function pointer to vkCreateAndroidSurfaceKHR");
			return false;
		}

		if(VK_SUCCESS != CreateAndroidSurfaceKHR(mApiInstance, &createInfo, nullptr, &mSurface))
		{
			core::Log::error("Unable to create android surface");
			return false;
		}
		core::Log::debug(" ----- /VulkanDriver::initSurface -----");
		return true;
#endif // ANDROID
	}

	//--------------------------------------------------------------------------------------------------------------
	void NativeFrameBufferVulkan::setupAttachmentDesc() {
		// Color attachment
		mAttachDesc.format = mImageFormat;
		mAttachDesc.samples = VK_SAMPLE_COUNT_1_BIT;
		mAttachDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		mAttachDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		mAttachDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		mAttachDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		mAttachDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		mAttachDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		// Depth attachment
		mDepthAttachDesc.format = VK_FORMAT_D32_SFLOAT;
		mDepthAttachDesc.samples = VK_SAMPLE_COUNT_1_BIT;
		mDepthAttachDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		mDepthAttachDesc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		mDepthAttachDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		mDepthAttachDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		mDepthAttachDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		mDepthAttachDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}

	//--------------------------------------------------------------------------------------------------------------
	bool NativeFrameBufferVulkan::createFrameBuffers(VkRenderPass _pass) {
		// Create frame buffers to store the views
		mSwapChainBuffers.resize(mSwapChainImageViews.size());
		for (size_t i = 0; i < mSwapChainImageViews.size(); i++) {
			VkImageView attachments[] = {
				(VkImageView)mSwapChainImageViews[i],
				mDepthImageView
			};

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = _pass;
			framebufferInfo.attachmentCount = 2;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = size().x;
			framebufferInfo.height = size().y;
			framebufferInfo.layers = 1;

			if(VK_SUCCESS != vkCreateFramebuffer(mDevice, &framebufferInfo, nullptr, &mSwapChainBuffers[i]))
				return false;
		}
		return true;
	}
}}