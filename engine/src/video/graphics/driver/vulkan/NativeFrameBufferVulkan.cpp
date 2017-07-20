//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "NativeFrameBufferVulkan.h"
#include <video/window/window.h>
#include <iostream>
#include <vector>

using namespace std;

namespace rev { namespace video {

	namespace {	// anonymous namespace for vulkan utilities

		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
			assert(!availableFormats.empty());
			// When format is undefined, it means we get to choose whatever format we want
			if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
				return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
			}

			// Prefer rgba8 + sRGB
			for (const auto& availableFormat : availableFormats) {
				if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
					return availableFormat;
				}
			}

			// Prefer rgba8
			for (const auto& availableFormat : availableFormats) {
				if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM) {
					return availableFormat;
				}
			}

			// Return watever we have
			return availableFormats[0];
		}

		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes) {
			return VK_PRESENT_MODE_FIFO_KHR;
		}
	}

	//--------------------------------------------------------------------------------------------------------------
	NativeFrameBufferVulkan::NativeFrameBufferVulkan(Window* _wnd, VkInstance _apiInstance, VulkanDriver* _driver)
		: mApiInstance(_apiInstance)
		, mDevice(_driver->device())
	{
#ifdef _WIN32
		if(!initSurface(_wnd, _apiInstance))
			return;
#else
		if(!initSurface())
			return;
#endif
		// Swap chain options
		auto support = _driver->querySwapChainSupport(mSurface);
		auto surfaceFormat = chooseSwapSurfaceFormat(support.formats);
		auto presentMode = chooseSwapPresentMode(support.presentModes);
		auto bufferExtent = support.capabilities.currentExtent;

		// Swap chain creation info
		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = mSurface;
		createInfo.minImageCount = 2; // To support double buffering
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = bufferExtent;
		createInfo.imageArrayLayers = 1; // Modify this for stereoscopic rendering
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		// TODO: Maybe support concurrent queue families (one for graphics, one for swap)
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional

		createInfo.preTransform = support.capabilities.currentTransform; // This means no transform
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		// Create the swap chain
		if (vkCreateSwapchainKHR(mDevice, &createInfo, nullptr, &mSwapChain) != VK_SUCCESS) {
			cout << "failed to create swap chain!";
			return;
		}
	}

	//--------------------------------------------------------------------------------------------------------------
	NativeFrameBufferVulkan::~NativeFrameBufferVulkan() {
		vkDestroySwapchainKHR(mDevice, mSwapChain, nullptr);
		vkDestroySurfaceKHR(mApiInstance, mSurface, nullptr);
	}

	//--------------------------------------------------------------------------------------------------------------
#ifdef _WIN32
	bool NativeFrameBufferVulkan::initSurface(Window* _wnd, VkInstance _apiInstance) {
		VkWin32SurfaceCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = _wnd->winapiHandle();
		createInfo.hinstance = GetModuleHandle(nullptr);

		// Get the extension
		auto CreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR) vkGetInstanceProcAddr(_apiInstance, "vkCreateWin32SurfaceKHR");

		// Create the surface
		if (!CreateWin32SurfaceKHR || CreateWin32SurfaceKHR(_apiInstance, &createInfo, nullptr, &mSurface) != VK_SUCCESS) {
			cout << "failed to create window surface!\n";
			return false;
		}

		return true;
	}
#endif // _WIN32
#ifdef ANDROID
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
		/*VkDisplayModeKHR

		VkDisplaySurfaceCreateInfoKHR	createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
		createInfo.*/
		LOGI("Finish Init surface");
		return true;
	}
#endif // ANDROID
}}