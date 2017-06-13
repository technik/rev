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

		private:
			void createInstance();
			void queryExtensions(VkInstanceCreateInfo&);

			VkInstance	mApiInstance;
			VkExtensionProperties * mExtensions = nullptr;
		};
} }
