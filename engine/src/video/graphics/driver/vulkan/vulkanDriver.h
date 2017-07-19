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
#ifdef ANDROID
			VulkanDriver();
#else
			VulkanDriver(Window* _wnd);
#endif
			~VulkanDriver();

		private:
			// Vulkan initialization
			void createInstance();
			bool checkValidationLayerSupport();
#ifdef _WIN32
			bool initSurface(Window* _wnd);
#else
			bool initSurface();
#endif
			void queryExtensions(VkInstanceCreateInfo&);
			void getPhysicalDevice();
			void findQueueFamilies();
			void createLogicalDevice();

			// Device
			VkInstance					mApiInstance;
			VkPhysicalDevice			mPhysicalDevice = VK_NULL_HANDLE;
			VkPhysicalDeviceProperties	mDeviceProps;
			VkPhysicalDeviceFeatures	mDeviceFeatures;
			VkExtensionProperties*		mExtensions = nullptr;
			VkDevice					mDevice;
			VkQueue						mGraphicsQueue;

			// Main window surface
			VkSurfaceKHR				mSurface;

			// Queues
			int		mQueueFamilyIndex = -1;

		};
} }
