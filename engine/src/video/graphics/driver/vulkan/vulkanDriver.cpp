//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#ifdef REV_USE_VULKAN
#include "vulkanDriver.h"

#define VK_USE_PLATFORM_ANDROID_KHR 1
#include <vulkan/vulkan.h>
#include <vulkan/vk_platform.h>

#include "NativeFrameBufferVulkan.h"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <set>

#ifdef ANDROID
#include <android/native_window.h>
#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "rev.AndroidPlayer", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "rev.AndroidPlayer", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "rev.AndroidPlayer", __VA_ARGS__))

#endif // ANDROID

using namespace std;
using namespace rev::math;

namespace // Anonymous namespace for vulkan utilities
{
	//----------------------------------------------------------------------------------------------------------
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_LUNARG_standard_validation"
	};

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	//----------------------------------------------------------------------------------------------------------
	bool isDeviceSuitable(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	//----------------------------------------------------------------------------------------------------------
	std::vector<const char*> getRequiredExtensions() {
		std::vector<const char*> extensions;
#ifdef _DEBUG
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

		return extensions;
	}

	//----------------------------------------------------------------------------------------------------------
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t obj,
		size_t location,
		int32_t code,
		const char* layerPrefix,
		const char* msg,
		void* userData) {

		std::cout << "validation layer: " << msg << std::endl;

		return VK_FALSE;
	}

	void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator) {
		auto func = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
		if (func != nullptr) {
			func(instance, callback, pAllocator);
		}
	}

	//--------------------------------------------------------------------------------------------------------------
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(rev::video::VulkanDriver::get().physicalDevice(), &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i))  && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		return uint32_t(-1);
	}
}

namespace rev {
	namespace video {

		// Static singleton data
		VulkanDriver* GraphicsDriverBase<VulkanDriver>::sInstance = nullptr;

		//--------------------------------------------------------------------------------------------------------------
#ifdef ANDROID
		VulkanDriver::VulkanDriver() {
#else
		VulkanDriver::VulkanDriver(const Window* _wnd) {
#endif
			createInstance();
#if _DEBUG
			setupDebugCallback();
#endif
			getPhysicalDevice();
			findQueueFamilies();
			createLogicalDevice();
			if(_wnd) {
				createNativeFrameBuffer(*_wnd);
			}
		}

		//--------------------------------------------------------------------------------------------------------------
		VulkanDriver::~VulkanDriver() {
			if(mNativeFB)
				delete mNativeFB;
			vkDestroyDevice(mDevice, nullptr);
			delete[] mExtensions;
			DestroyDebugReportCallbackEXT(mApiInstance, mDebugCallback, nullptr);
			vkDestroyInstance(mApiInstance, nullptr);
		}

		//--------------------------------------------------------------------------------------------------------------
		Mat44f VulkanDriver::projectionMtx(float _fov, float _aspectRatio, float _nearClip, float _farClip) {
			Mat44f proj;
			float invRange = 1.f/(_farClip - _nearClip);
			float itgX = 1.f/tan(_fov*0.5f);
			float itgY = itgX/_aspectRatio;

			proj[0] = Vec4f(itgX, 0.f, 0.f, 0.f);
			proj[1] = Vec4f(0.f, 0.f, -itgY, 0.f);
			proj[2] = Vec4f(0.f, _farClip*invRange, 0.f, -_farClip*_nearClip*invRange);
			proj[3] = Vec4f(0.f, 1.f, 0.f, 0.f);
			return proj;
		}

		//--------------------------------------------------------------------------------------------------------------
		NativeFrameBufferVulkan* VulkanDriver::createNativeFrameBuffer(const Window& _wnd) {
			mNativeFB = new NativeFrameBufferVulkan(_wnd, mApiInstance, *this);
			return mNativeFB;
		}

		//--------------------------------------------------------------------------------------------------------------
		VkCommandPool VulkanDriver::createCommandPool(bool _resetOften) const {
			VkCommandPoolCreateInfo poolInfo = {};
			poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolInfo.queueFamilyIndex = mQueueFamilyIndex;
			poolInfo.flags = _resetOften?(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT|VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT):0; // Optional

			VkCommandPool commandPool;
			if (vkCreateCommandPool(mDevice, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
				cout <<"failed to create command pool!\n";
			}

			return commandPool;
		}

		//--------------------------------------------------------------------------------------------------------------
		void VulkanDriver::createBuffer(VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _properties, VkBuffer& _buffer, VkDeviceMemory& _bufferMemory) const 
		{
			VkBufferCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			createInfo.size = _size;
			createInfo.usage = _usage;
			createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VkDevice device(mDevice);
			vkCreateBuffer(mDevice, &createInfo, nullptr, &_buffer);

			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(mDevice, _buffer, &memRequirements);

			VkMemoryAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, _properties);

			vkAllocateMemory(mDevice, &allocInfo, nullptr, &_bufferMemory);
			vkBindBufferMemory(mDevice, _buffer, _bufferMemory, 0);
		}

		//--------------------------------------------------------------------------------------------------------------
		VulkanDriver::SwapChainSupportDetails VulkanDriver::querySwapChainSupport(VkSurfaceKHR _surface) const {
			SwapChainSupportDetails details;
			// Capabilities
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mPhysicalDevice, _surface, &details.capabilities);
			// Formats
			uint32_t formatCount;
			vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, _surface, &formatCount, nullptr);

			if (formatCount > 0) {
				details.formats.resize(formatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, _surface, &formatCount, details.formats.data());
			}
			// Present modes
			uint32_t presentModeCount;
			vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, _surface, &presentModeCount, nullptr);

			if (presentModeCount > 0) {
				details.presentModes.resize(presentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice, _surface, &presentModeCount, details.presentModes.data());
			}

			return details;
		}

		//--------------------------------------------------------------------------------------------------------------
		void VulkanDriver::createInstance() {
			if(!checkValidationLayerSupport())
				return;

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
#ifdef _DEBUG
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
#else
			createInfo.enabledLayerCount = 0;
#endif
			auto extensions = getRequiredExtensions();
			createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
			createInfo.ppEnabledExtensionNames = extensions.data();

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
		bool VulkanDriver::checkValidationLayerSupport() {
			uint32_t layerCount;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

			std::vector<VkLayerProperties> availableLayers(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
			for (const char* layerName : validationLayers) {
				bool layerFound = false;

				for (const auto& layerProperties : availableLayers) {
					if (strcmp(layerName, layerProperties.layerName) == 0) {
						layerFound = true;
						break;
					}
				}

				if (!layerFound) {
					assert(false);
					return false;
				}
			}

			return true;
		}

		//----------------------------------------------------------------------------------------------------------
		void VulkanDriver::setupDebugCallback() {
			VkDebugReportCallbackCreateInfoEXT createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
			createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
			createInfo.pfnCallback = debugCallback;

			auto func = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(mApiInstance, 
																"vkCreateDebugReportCallbackEXT");
			if ((func == nullptr)
			  ||(func(mApiInstance, &createInfo, nullptr, &mDebugCallback) != VK_SUCCESS))
			{
				cout << "Warning: Unable to set Vulkan debug callback.\n"
					<< "You won't be receiving notifications from validation layers\n";
				return;
			}
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

			vector<VkPhysicalDevice> devices(deviceCount);
			vkEnumeratePhysicalDevices(mApiInstance, &deviceCount, devices.data());
			for(auto dev : devices) {
				if(isDeviceSuitable(dev)) {
					mPhysicalDevice = dev;
					break;
				}
			}

			// Get device properties
			vkGetPhysicalDeviceProperties(mPhysicalDevice, &mDeviceProps);
			cout << "Vulkan device name: " << mDeviceProps.deviceName << "\n";
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
			createInfo.pEnabledFeatures = &mDeviceFeatures;

			createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
			createInfo.ppEnabledExtensionNames = deviceExtensions.data();
#if _DEBUG
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
#else
			createInfo.enabledLayerCount = 0;
#endif

			if (vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice) != VK_SUCCESS) {
				 //LOGE("failed to create logical device!");
			}

			vkGetDeviceQueue(mDevice, mQueueFamilyIndex, 0, &mGraphicsQueue);
		}
	}
}

#endif // REV_USE_VULKAN