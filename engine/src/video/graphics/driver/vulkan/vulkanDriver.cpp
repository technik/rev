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
#include <core/log.h>
#include <core/platform/fileSystem/file.h>

#ifdef ANDROID
#include <android/native_window.h>

#endif // ANDROID

using namespace std;
using namespace rev::core;
using namespace rev::math;

namespace // Anonymous namespace for vulkan utilities
{

	VkShaderModule createShaderModule(VkDevice device, const File& code) {
		// Ensure code is aligned before we recast
		std::vector<char> alignedCode(code.sizeInBytes());
		for(size_t i = 0; i < code.sizeInBytes(); ++i) {
			alignedCode[i] = code.bufferAsText()[i];
		}

		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = alignedCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(alignedCode.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			cout << "failed to create shader module!\n";
			return VK_NULL_HANDLE;
		}

		return shaderModule;
	}

	//----------------------------------------------------------------------------------------------------------
	const std::vector<const char*> validationLayers = {
#ifdef ANDROID
#else
		"VK_LAYER_LUNARG_standard_validation"
#endif
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
		template<>
		VulkanDriver* GraphicsDriverBase<VulkanDriver>::sInstance = nullptr;

		//--------------------------------------------------------------------------------------------------------------
		VulkanDriver::VulkanDriver(const Window* _wnd) {
			core::Log::debug(" ----- VulkanDriver::VulkanDriver -----");
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
			core::Log::debug(" ----- /VulkanDriver::VulkanDriver -----");
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
			float itgY = itgX*_aspectRatio;

			proj[0] = Vec4f(itgX, 0.f, 0.f, 0.f);
			proj[1] = Vec4f(0.f, 0.f, -itgY, 0.f);
			proj[2] = Vec4f(0.f, _farClip*invRange, 0.f, -_farClip*_nearClip*invRange);
			proj[3] = Vec4f(0.f, 1.f, 0.f, 0.f);
			return proj;
		}

		//--------------------------------------------------------------------------------------------------------------
		NativeFrameBufferVulkan* VulkanDriver::createNativeFrameBuffer(const Window& _wnd) {
			mNativeFB = new NativeFrameBufferVulkan(_wnd, mApiInstance);
			return mNativeFB;
		}

		//--------------------------------------------------------------------------------------------------------------
		VkPipeline VulkanDriver::createPipeline(const VkExtent2D& _viewportSize, VkRenderPass _renderPass, const VertexFormat& _vtxFmt, VkPipelineLayout _pipelineLayout) {
			// ----- Create a graphics pipeline -----
			auto vertexShader = core::File("data/vert.spv");
			auto pixelShader = core::File("data/frag.spv");

			VkShaderModule vertShaderModule = createShaderModule(mDevice, vertexShader);
			VkShaderModule fragShaderModule = createShaderModule(mDevice, pixelShader);
			// Vertex shader stage
			VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
			vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			vertShaderStageInfo.module = vertShaderModule;
			vertShaderStageInfo.pName = "main";
			// Fragment shader stage
			VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
			fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragShaderStageInfo.module = fragShaderModule;
			fragShaderStageInfo.pName = "main";

			VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

			// Vertex input stage
			VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

			auto bindingDescription = _vtxFmt.getBindingDescription();
			auto attributeDescriptions = _vtxFmt.getAttributeDescriptions();

			vertexInputInfo.vertexBindingDescriptionCount = 1;
			vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
			vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
			vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

			// Topology
			VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
			inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			inputAssembly.primitiveRestartEnable = VK_FALSE;

			// Viewport
			VkViewport viewport = {};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float)_viewportSize.width;
			viewport.height = (float) _viewportSize.height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			// Scissor
			VkRect2D scissor = {};
			scissor.offset = {0, 0};
			scissor.extent = _viewportSize;

			VkPipelineViewportStateCreateInfo viewportState = {};
			viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportState.viewportCount = 1;
			viewportState.pViewports = &viewport;
			viewportState.scissorCount = 1;
			viewportState.pScissors = &scissor;

			// Rasterizer
			VkPipelineRasterizationStateCreateInfo rasterizer = {};
			rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizer.depthClampEnable = VK_FALSE;
			rasterizer.rasterizerDiscardEnable = VK_FALSE;
			rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
			rasterizer.lineWidth = 1.0f;
			rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
			rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
			rasterizer.depthBiasEnable = VK_FALSE;
			rasterizer.depthBiasConstantFactor = 0.0f; // Optional
			rasterizer.depthBiasClamp = 0.0f; // Optional
			rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
			VkPipelineMultisampleStateCreateInfo multisampling = {};
			multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampling.sampleShadingEnable = VK_FALSE;
			multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			multisampling.minSampleShading = 1.0f; // Optional
			multisampling.pSampleMask = nullptr; // Optional
			multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
			multisampling.alphaToOneEnable = VK_FALSE; // Optional

													   // Color blending
			VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
			colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			colorBlendAttachment.blendEnable = VK_FALSE;
			colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
			colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
			colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
			colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
			colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
			colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

			VkPipelineColorBlendStateCreateInfo colorBlending = {};
			colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlending.logicOpEnable = VK_FALSE;
			colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
			colorBlending.attachmentCount = 1;
			colorBlending.pAttachments = &colorBlendAttachment;
			colorBlending.blendConstants[0] = 0.0f; // Optional
			colorBlending.blendConstants[1] = 0.0f; // Optional
			colorBlending.blendConstants[2] = 0.0f; // Optional
			colorBlending.blendConstants[3] = 0.0f; // Optional

													// Pipeline layout
			VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutInfo.setLayoutCount = 0;//1; // Optional
			pipelineLayoutInfo.pSetLayouts = 0;//&mDescriptorSetLayout; // Optional
			pipelineLayoutInfo.pushConstantRangeCount = 1; // Optional
			auto range = VkPushConstantRange{
				VK_SHADER_STAGE_VERTEX_BIT,
				0,
				4*4*sizeof(float)
			};
			pipelineLayoutInfo.pPushConstantRanges = &range;//0; // Optional

			if (vkCreatePipelineLayout(mDevice, &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
				cout << "failed to create pipeline layout!\n";
				return false;
			}

			// Actual pipeline
			VkGraphicsPipelineCreateInfo pipelineInfo = {};
			pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineInfo.stageCount = 2;
			pipelineInfo.pStages = shaderStages;

			pipelineInfo.pVertexInputState = &vertexInputInfo;
			pipelineInfo.pInputAssemblyState = &inputAssembly;
			pipelineInfo.pViewportState = &viewportState;
			pipelineInfo.pRasterizationState = &rasterizer;
			pipelineInfo.pMultisampleState = &multisampling;
			pipelineInfo.pDepthStencilState = nullptr; // Optional
			pipelineInfo.pColorBlendState = &colorBlending;
			pipelineInfo.pDynamicState = nullptr; // Optional

			pipelineInfo.layout = _pipelineLayout;

			pipelineInfo.renderPass = _renderPass;
			pipelineInfo.subpass = 0;

			pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
			pipelineInfo.basePipelineIndex = -1; // Optional

			VkPipeline pipeline = VK_NULL_HANDLE;
			vkCreateGraphicsPipelines(mDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);

			vkDestroyShaderModule(mDevice, fragShaderModule, nullptr);
			vkDestroyShaderModule(mDevice, vertShaderModule, nullptr);

			return pipeline;
		}

		//--------------------------------------------------------------------------------------------------------------
		VkCommandPool VulkanDriver::createCommandPool(bool _resetOften) const {
			core::Log::debug(" ----- /VulkanDriver::createCommandPool -----");
			VkCommandPoolCreateInfo poolInfo = {};
			poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolInfo.queueFamilyIndex = mQueueFamilyIndex;
			poolInfo.flags = _resetOften?(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT|VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT):0; // Optional

			VkCommandPool commandPool;
			if (vkCreateCommandPool(mDevice, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
				Log::error("failed to create command pool!\n");
			}

			core::Log::debug(" ----- /VulkanDriver::createCommandPool -----");
			return commandPool;
		}

		//--------------------------------------------------------------------------------------------------------------
		void VulkanDriver::createBuffer(VkDeviceSize _size, VkBufferUsageFlags _usage, VkMemoryPropertyFlags _properties, VkBuffer& _buffer, VkDeviceMemory& _bufferMemory) const 
		{
			core::Log::debug(" ----- VulkanDriver::createBuffer -----");
			VkBufferCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			createInfo.size = _size;
			createInfo.usage = _usage;
			createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			vkCreateBuffer(mDevice, &createInfo, nullptr, &_buffer);

			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(mDevice, _buffer, &memRequirements);

			VkMemoryAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, _properties);

			vkAllocateMemory(mDevice, &allocInfo, nullptr, &_bufferMemory);
			vkBindBufferMemory(mDevice, _buffer, _bufferMemory, 0);
			core::Log::debug(" ----- /VulkanDriver::createBuffer -----");
		}

		//--------------------------------------------------------------------------------------------------------------
		VulkanDriver::SwapChainSupportDetails VulkanDriver::querySwapChainSupport(VkSurfaceKHR _surface) const {
			core::Log::debug(" ----- VulkanDriver::querySwapChainSupport -----");
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

			core::Log::debug(" ----- /VulkanDriver::querySwapChainSupport -----");
			return details;
		}

		//--------------------------------------------------------------------------------------------------------------
		void VulkanDriver::createInstance() {
			core::Log::debug(" ----- VulkanDriver::createInstance -----");
#ifndef ANDROID
			if(!checkValidationLayerSupport())
				return;
#endif

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
				core::Log::debug("Error: Unable to create vulkan instance.\n");
			}
			core::Log::debug(" ----- /VulkanDriver::createInstance -----");
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
			core::Log::debug(" ----- VulkanDriver::queryExtensions -----");
			// Query available extensions count
			vkEnumerateInstanceExtensionProperties(nullptr, &_ci.enabledExtensionCount, nullptr);
			Log::info("Found %d vulkan extensions: %d\n" , _ci.enabledExtensionCount);

			// Allocate space for extension names
			mExtensions = new VkExtensionProperties[_ci.enabledExtensionCount];
			char** extensionNames = new char*[_ci.enabledExtensionCount];
			_ci.ppEnabledExtensionNames = extensionNames;
			vkEnumerateInstanceExtensionProperties(nullptr, &_ci.enabledExtensionCount, mExtensions);

			// Copy extension names into createInfo
			for (size_t i = 0; i < _ci.enabledExtensionCount; ++i) {
				extensionNames[i] = mExtensions[i].extensionName;
				Log::info((std::string("- ") + mExtensions[i].extensionName).c_str());
			}
			core::Log::debug(" ----- /VulkanDriver::queryExtensions -----");
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
			Log::info("Vulkan device name: %s", mDeviceProps.deviceName);
			vkGetPhysicalDeviceFeatures(mPhysicalDevice, &mDeviceFeatures);
		}

		//--------------------------------------------------------------------------------------------------------------
		void VulkanDriver::findQueueFamilies() {
			core::Log::debug(" ----- VulkanDriver::findQueueFamilies -----");
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
			core::Log::debug(" ----- /VulkanDriver::findQueueFamilies -----");
		}

		//--------------------------------------------------------------------------------------------------------------
		void VulkanDriver::createLogicalDevice() {
			core::Log::debug(" ----- VulkanDriver::createLogicalDevice -----");
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
				Log::error("failed to create logical device!");
			}

			vkGetDeviceQueue(mDevice, mQueueFamilyIndex, 0, &mGraphicsQueue);
			core::Log::debug(" ----- /VulkanDriver::createLogicalDevice -----");
		}
	}
}

#endif // REV_USE_VULKAN