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

		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes) {
			return VK_PRESENT_MODE_FIFO_KHR;
		}
	}

	//--------------------------------------------------------------------------------------------------------------
	NativeFrameBufferVulkan::NativeFrameBufferVulkan(const Window& _wnd, VkInstance _apiInstance)
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
			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = image;
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = mImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			VkImageView view;
			if(vkCreateImageView(mDevice, &createInfo, nullptr, &view) != VK_SUCCESS) {
				core::Log::error("failed to create image views within the native frame buffer\n");
			}
			mSwapChainImageViews.push_back(view);
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
	/*
	bool VulkanDriver::initSurface(VkInstance _apiInstance) {
		LOGI("Init surface");
		// Get display information
		//uint32_t displayPropCount = 0;
		//vkGetPhysicalDeviceDisplayPropertiesKHR(mPhysicalDevice, &displayPropCount, nullptr);

		// Get extension
		auto fpCreateAndroidSurfaceKHR = (PFN_vkCreateDisplayPlaneSurfaceKHR)vkGetInstanceProcAddr(_apiInstance, "vkCreateAndroidSurfaceKHR");
		if (fpCreateAndroidSurfaceKHR == nullptr) {
			//LOGE("Unable to get create surface extension");
			return false;
		}

		// Set display mode
		VkDisplayModeKHR

		VkDisplaySurfaceCreateInfoKHR	createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
		createInfo.
		LOGI("Finish Init surface");
		return true;
	}*/

	//--------------------------------------------------------------------------------------------------------------
	void NativeFrameBufferVulkan::setupAttachmentDesc() {
		mAttachDesc = {};
		mAttachDesc.format = mImageFormat;
		mAttachDesc.samples = VK_SAMPLE_COUNT_1_BIT;
		mAttachDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		mAttachDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		mAttachDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		mAttachDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		mAttachDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		mAttachDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	}
}}