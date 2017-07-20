//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include <vulkan/vulkan.h>

namespace rev {
	namespace video {

		class Window;
		class NativeFrameBufferVulkan;

		class VulkanDriver {
		public:
#ifdef ANDROID
			VulkanDriver();
#else
			// If _wnd is not null, the driver will try to create a native frame buffer for that window
			VulkanDriver(Window* _wnd = nullptr);
#endif
			~VulkanDriver();

			NativeFrameBufferVulkan* createNativeFrameBuffer(Window* _wnd = nullptr);
			// Vulkan driver supports window-less contexts, so a nativeFrameBuffer may not exist
			NativeFrameBufferVulkan* nativeFrameBuffer() const { return mNativeFB; }

		private:
			// Vulkan initialization
			void createInstance();
			bool checkValidationLayerSupport();
			void queryExtensions(VkInstanceCreateInfo&);
			void setupDebugCallback();
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

			// Main window
			NativeFrameBufferVulkan*	mNativeFB = nullptr;

			// Queues
			int		mQueueFamilyIndex = -1;

#ifdef _DEBUG
			VkDebugReportCallbackEXT mDebugCallback;
#endif 

		};
} }
