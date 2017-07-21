//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include <vulkan/vulkan.h>
#include <vector>

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
			VulkanDriver(const Window* _wnd = nullptr);
#endif
			~VulkanDriver();

			VkDevice device() const { return mDevice; }
			VkPhysicalDevice physicalDevice() const { return mPhysicalDevice; }
			VkQueue graphicsQueue() const { return mGraphicsQueue; }
			int graphicsFamily() const { return mQueueFamilyIndex; }

			NativeFrameBufferVulkan* createNativeFrameBuffer(const Window& _wnd);
			// Vulkan driver supports window-less contexts, so a nativeFrameBuffer may not exist
			NativeFrameBufferVulkan* nativeFrameBuffer() const { return mNativeFB; }

			// Driver capabilities
			struct SwapChainSupportDetails {
				VkSurfaceCapabilitiesKHR capabilities;
				std::vector<VkSurfaceFormatKHR> formats;
				std::vector<VkPresentModeKHR> presentModes;
			};

			SwapChainSupportDetails querySwapChainSupport(VkSurfaceKHR surface) const;

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
