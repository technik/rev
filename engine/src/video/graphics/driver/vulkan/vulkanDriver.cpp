//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "vulkanDriver.h"

#include <vulkan/vulkan.h>

#include <video/window/window.h>
#include <iostream>

#ifdef ANDROID
#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "rev.AndroidPlayer", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "rev.AndroidPlayer", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "rev.AndroidPlayer", __VA_ARGS__))
#endif // ANDROID

namespace rev {
	namespace video {

		//--------------------------------------------------------------------------------------------------------------
#ifdef ANDROID
		VulkanDriver::VulkanDriver() {
#else
		VulkanDriver::VulkanDriver(Window* _wnd) {
#endif
			LOGI("---------------Vulkan Driver Construction----------------------");
			createInstance();
			getPhysicalDevice();
		}

		//--------------------------------------------------------------------------------------------------------------
		VulkanDriver::~VulkanDriver() {
			delete[] mExtensions;
			vkDestroyInstance(mApiInstance, nullptr);
		}

		//--------------------------------------------------------------------------------------------------------------
		void VulkanDriver::createInstance() {
			// Basic application info
			VkApplicationInfo appInfo = {};
			appInfo.apiVersion = VK_API_VERSION_1_0;
			appInfo.pEngineName = "RevEngine";
			appInfo.pApplicationName = "RevPlayer";
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			// Instance creation info
			VkInstanceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.pApplicationInfo = &appInfo;
			createInfo.enabledLayerCount = 0;

			// Query extensions
			queryExtensions(createInfo);

			VkResult res = vkCreateInstance(&createInfo, nullptr, &mApiInstance);
			if (res != VK_SUCCESS)
			{

				LOGE("Error: Unable to create vulkan instance.\n");
			}
		}

		//--------------------------------------------------------------------------------------------------------------
		void VulkanDriver::queryExtensions(VkInstanceCreateInfo& _ci) {
			// Query available extensions count
			vkEnumerateInstanceExtensionProperties(nullptr, &_ci.enabledExtensionCount, nullptr);
			LOGI("Found %d vulkan extensions:\n" , _ci.enabledExtensionCount);

			// Allocate space for extension names
			mExtensions = new VkExtensionProperties[_ci.enabledExtensionCount];
			char** extensionNames = new char*[_ci.enabledExtensionCount];
			_ci.ppEnabledExtensionNames = extensionNames;
			vkEnumerateInstanceExtensionProperties(nullptr, &_ci.enabledExtensionCount, mExtensions);

			// Copy extension names into createInfo
			for (size_t i = 0; i < _ci.enabledExtensionCount; ++i) {
				extensionNames[i] = mExtensions[i].extensionName;
				std::cout << "- " << mExtensions[i].extensionName << "\n";
			}
		}

		//--------------------------------------------------------------------------------------------------------------
		void VulkanDriver::getPhysicalDevice() {
			uint32_t deviceCount = 0;
			vkEnumeratePhysicalDevices(mApiInstance, &deviceCount, nullptr);
			LOGI("Vulkan: Found  %d physical devices\n", deviceCount);

			VkPhysicalDevice* devices = new VkPhysicalDevice[deviceCount];
			vkEnumeratePhysicalDevices(mApiInstance, &deviceCount, devices);	
			mDevice = devices[0];
			delete[] devices;

			// Get device properties
			vkGetPhysicalDeviceProperties(mDevice, &mDeviceProps);
			LOGI("Vulkan device name: %s", mDeviceProps.deviceName);
			vkGetPhysicalDeviceFeatures(mDevice, &mDeviceFeatures);
		}
	}
}
