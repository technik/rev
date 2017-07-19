//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#ifdef REV_USE_VULKAN
#include "vulkanDriver.h"

#define VK_USE_PLATFORM_ANDROID_KHR 1
#include <vulkan/vulkan.h>
#include <vulkan/vk_platform.h>

#include <video/window/window.h>
#include <iostream>
#include <string>
#include <vector>

#ifdef ANDROID
#include <android/native_window.h>
#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "rev.AndroidPlayer", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "rev.AndroidPlayer", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "rev.AndroidPlayer", __VA_ARGS__))

#endif // ANDROID

using namespace std;

namespace rev {
	namespace video {

		//--------------------------------------------------------------------------------------------------------------
#ifdef ANDROID
		VulkanDriver::VulkanDriver() {
#else
		VulkanDriver::VulkanDriver() {
#endif
			//LOGI("---------------Vulkan Driver Construction-------------------------------");
			createInstance();
			initSurface();
			getPhysicalDevice();
			findQueueFamilies();
			createLogicalDevice();
			//LOGI("---------------Finished Vulkan Driver Construction----------------------");
		}

		//--------------------------------------------------------------------------------------------------------------
		VulkanDriver::~VulkanDriver() {
			vkDestroyDevice(mDevice, nullptr);
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
				cout << "Error: Unable to create vulkan instance.\n";
				//LOGE("Error: Unable to create vulkan instance.\n");
			}
		}

		//--------------------------------------------------------------------------------------------------------------
		void VulkanDriver::initSurface() {
			//LOGI("Init surface");
			// Get display information
			//uint32_t displayPropCount = 0;
			//vkGetPhysicalDeviceDisplayPropertiesKHR(mPhysicalDevice, &displayPropCount, nullptr);

			// Get extension
			auto fpCreateAndroidSurfaceKHR = (PFN_vkCreateDisplayPlaneSurfaceKHR)vkGetInstanceProcAddr(mApiInstance, "vkCreateAndroidSurfaceKHR");
			if (fpCreateAndroidSurfaceKHR == nullptr) {
				//LOGE("Unable to get create surface extension");
				return;
			}

			// Set display mode
			/*VkDisplayModeKHR

			VkDisplaySurfaceCreateInfoKHR	createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
			createInfo.*/
			//LOGI("Finish Init surface");
		}

		//--------------------------------------------------------------------------------------------------------------
		void VulkanDriver::queryExtensions(VkInstanceCreateInfo& _ci) {
			// Query available extensions count
			vkEnumerateInstanceExtensionProperties(nullptr, &_ci.enabledExtensionCount, nullptr);
			//LOGI("Found %d vulkan extensions:\n" , _ci.enabledExtensionCount);

			// Allocate space for extension names
			mExtensions = new VkExtensionProperties[_ci.enabledExtensionCount];
			char** extensionNames = new char*[_ci.enabledExtensionCount];
			_ci.ppEnabledExtensionNames = extensionNames;
			vkEnumerateInstanceExtensionProperties(nullptr, &_ci.enabledExtensionCount, mExtensions);

			// Copy extension names into createInfo
			for (size_t i = 0; i < _ci.enabledExtensionCount; ++i) {
				extensionNames[i] = mExtensions[i].extensionName;
#if _DEBUG
				cout << "- " << mExtensions[i].extensionName << "\n";
#endif
				//LOGI((std::string("- ") + mExtensions[i].extensionName).c_str());
			}
		}

		//--------------------------------------------------------------------------------------------------------------
		void VulkanDriver::getPhysicalDevice() {
			uint32_t deviceCount = 0;
			vkEnumeratePhysicalDevices(mApiInstance, &deviceCount, nullptr);
			//LOGI("Vulkan: Found  %d physical devices\n", deviceCount);

			VkPhysicalDevice* devices = new VkPhysicalDevice[deviceCount];
			vkEnumeratePhysicalDevices(mApiInstance, &deviceCount, devices);	
			mPhysicalDevice = devices[0];
			delete[] devices;

			// Get device properties
			vkGetPhysicalDeviceProperties(mPhysicalDevice, &mDeviceProps);
			//LOGI("Vulkan device name: %s", mDeviceProps.deviceName);
			vkGetPhysicalDeviceFeatures(mPhysicalDevice, &mDeviceFeatures);
		}

		//--------------------------------------------------------------------------------------------------------------
		void VulkanDriver::findQueueFamilies() {
			uint32_t familyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &familyCount, nullptr);

			std::vector<VkQueueFamilyProperties>	families(familyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &familyCount, families.data());

			int index = 0;
			for (const auto& family : families) {
				if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT && family.queueCount > 0)
					mQueueFamilyIndex = index;

				++index;
			}
			
			assert(mQueueFamilyIndex >= 0);
		}

		//--------------------------------------------------------------------------------------------------------------
		void VulkanDriver::createLogicalDevice() {
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = mQueueFamilyIndex;
			queueCreateInfo.queueCount = 1;
			float queuePriority = 1.0f;
			queueCreateInfo.pQueuePriorities = &queuePriority;

			VkDeviceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			createInfo.pQueueCreateInfos = &queueCreateInfo;
			createInfo.queueCreateInfoCount = 1;
			VkPhysicalDeviceFeatures deviceFeatures = {};
			createInfo.pEnabledFeatures = &deviceFeatures;
			createInfo.enabledExtensionCount = 0;
			createInfo.enabledLayerCount = 0;

			if (vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice) != VK_SUCCESS) {
				 //LOGE("failed to create logical device!");
			}

			vkGetDeviceQueue(mDevice, mQueueFamilyIndex, 0, &mGraphicsQueue);
		}
	}
}

#endif // REV_USE_VULKAN