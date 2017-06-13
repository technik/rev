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
		}

		//--------------------------------------------------------------------------------------------------------------
		void VulkanDriver::createInstance() {
			// Basic application info
			VkApplicationInfo appInfo;
			appInfo.apiVersion = VK_API_VERSION_1_0;
			appInfo.pEngineName = "RevEngine";
			appInfo.pApplicationName = "RevPlayer";
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			// Instance creation info
			VkInstanceCreateInfo createInfo;
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.pApplicationInfo = &appInfo;

			// Query extensions
			uint32_t extensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

			std::cout << "Found " << extensionCount << " vulkan extensions:\n";

			constexpr size_t MAX_EXTENSIONS = 32;
			extensionCount = std::min(extensionCount, MAX_EXTENSIONS);
			VkExtensionProperties extensions[MAX_EXTENSIONS];
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions);

			for (size_t i = 0; i < extensionCount; ++i) {
				std::cout << "- " << extensions[i].extensionName << "\n";
			}
		}
	}
}
