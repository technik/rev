//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "../GraphicsDriverBase.h"
#include <math/algebra/matrix.h>
#include <video/graphics/geometry/VertexFormat.h>
#include <video/graphics/driver/vulkan/NativeFrameBufferVulkan.h>
#ifdef ANDROID
#include <core/platform/platformInfo.h>
#endif

namespace rev {
	namespace video {

		class Window;

		class VulkanDriver : public GraphicsDriverBase<VulkanDriver> {
		public:
			VulkanDriver(const Window* _wnd = nullptr);
			~VulkanDriver();

			// API dependent math
			/// \param _fov horizontal field of view in radians
			/// \param _aspectRatio horizontal fov / vertical fov
			static math::Mat44f projectionMtx(float _fov, float _aspectRatio, float _nearClip, float _farClip);

			VkDevice device() const { return mDevice; }
			VkPhysicalDevice physicalDevice() const { return mPhysicalDevice; }
			VkQueue graphicsQueue() const { return mGraphicsQueue; }
			int graphicsFamily() const { return mQueueFamilyIndex; }

			NativeFrameBufferVulkan* createNativeFrameBuffer(const Window& _wnd, NativeFrameBufferVulkan::ZBufferFormat _zFormat);
			// Vulkan driver supports window-less contexts, so a nativeFrameBuffer may not exist
			NativeFrameBufferVulkan* nativeFrameBuffer() const { return mNativeFB; }

			VkPipeline createPipeline(const VkExtent2D& _viewportSize, VkRenderPass, const VertexFormat& _vtxFmt, VkPipelineLayout _pipelineLayout);
			VkCommandPool createCommandPool(bool _resetOften) const;
			
			void createBuffer(VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) const;
			void createImage(const math::Vec2u& _size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) const;
			VkImageView createImageView(VkImage _image, VkFormat _format, VkImageAspectFlags _aspectFlags) const;

			// Driver capabilities
			struct SwapChainSupportDetails {
				VkSurfaceCapabilitiesKHR capabilities;
				std::vector<VkSurfaceFormatKHR> formats;
				std::vector<VkPresentModeKHR> presentModes;
			};

			SwapChainSupportDetails querySwapChainSupport(VkSurfaceKHR surface) const;
			void synch();

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
