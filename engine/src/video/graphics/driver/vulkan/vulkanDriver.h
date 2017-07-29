//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "../GraphicsDriverBase.h"
#include <math/algebra/matrix.h>

namespace rev {
	namespace video {

		class Window;
		class NativeFrameBufferVulkan;

		class VulkanDriver : public GraphicsDriverBase<VulkanDriver> {
		public:
#ifdef ANDROID
			VulkanDriver();
#else
			// If _wnd is not null, the driver will try to create a native frame buffer for that window
			VulkanDriver(const Window* _wnd = nullptr);
#endif
			~VulkanDriver();

			// API dependent math
			/// \param _fov horizontal field of view in radians
			/// \param _aspectRatio horizontal fov / vertical fov
			static math::Mat44f projectionMtx(float _fov, float _aspectRatio, float _nearClip, float _farClip);

			VkDevice device() const { return mDevice; }
			VkPhysicalDevice physicalDevice() const { return mPhysicalDevice; }
			VkQueue graphicsQueue() const { return mGraphicsQueue; }
			int graphicsFamily() const { return mQueueFamilyIndex; }

			NativeFrameBufferVulkan* createNativeFrameBuffer(const Window& _wnd);
			// Vulkan driver supports window-less contexts, so a nativeFrameBuffer may not exist
			NativeFrameBufferVulkan* nativeFrameBuffer() const { return mNativeFB; }

			VkCommandPool createCommandPool(bool _resetOften) const;
			void createBuffer(VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) const;

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
