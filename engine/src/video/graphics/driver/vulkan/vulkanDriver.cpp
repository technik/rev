//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "vulkanDriver.h"

#include <vulkan/vulkan.h>

#include <video/window/window.h>
#include <iostream>

namespace rev {
	namespace video {

		//--------------------------------------------------------------------------------------------------------------
		VulkanDriver::VulkanDriver(Window* _wnd) {
			createInstance();
			getPhysicalDevice();
		}

		//--------------------------------------------------------------------------------------------------------------
		VulkanDriver::~VulkanDriver() {
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
				std::cout << "Error: Unable to create vulkan instance.\n";
			}
		}

		//--------------------------------------------------------------------------------------------------------------
		void VulkanDriver::queryExtensions(VkInstanceCreateInfo& _ci) {
			// Query available extensions count
			vkEnumerateInstanceExtensionProperties(nullptr, &_ci.enabledExtensionCount, nullptr);
			std::cout << "Found " << _ci.enabledExtensionCount << " vulkan extensions:\n";

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
			std::cout << "Vulkan: Found " << deviceCount << " physical devices\n";
		}
	}
}
