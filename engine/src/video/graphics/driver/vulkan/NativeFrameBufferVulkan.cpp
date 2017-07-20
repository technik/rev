//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "NativeFrameBufferVulkan.h"
#include <video/window/window.h>
#include <iostream>

using namespace std;

namespace rev { namespace video {

	//--------------------------------------------------------------------------------------------------------------
	NativeFrameBufferVulkan::NativeFrameBufferVulkan(Window* _wnd, VkInstance _apiInstance)
		:mApiInstance(_apiInstance)
	{
#ifdef _WIN32
		if(!initSurface(_wnd, _apiInstance))
			return;
#else
		if(!initSurface())
			return;
#endif
	}

	//--------------------------------------------------------------------------------------------------------------
	NativeFrameBufferVulkan::~NativeFrameBufferVulkan() {
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