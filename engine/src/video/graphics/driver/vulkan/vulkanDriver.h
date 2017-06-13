//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include <vulkan/vulkan.h>

namespace rev {
	namespace video {

		class Window;

		class VulkanDriver {
		public:
			VulkanDriver(Window*);
			~VulkanDriver();

		private:
			void createInstance();
			void queryExtensions(VkInstanceCreateInfo&);
			void getPhysicalDevice();

			VkInstance			mApiInstance;
			VkPhysicalDevice	mDevice = VK_NULL_HANDLE;
			VkExtensionProperties * mExtensions = nullptr;
		};
} }
