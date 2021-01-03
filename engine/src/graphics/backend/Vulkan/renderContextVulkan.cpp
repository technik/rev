//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2020 Carmelo J Fdez-Aguera
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#include "renderContextVulkan.h"
#include "../Windows/windowsPlatform.h"
#include <core/platform/osHandler.h>

#include <iostream>

namespace rev::gfx {

	namespace {
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData) {

			std::cout << "validation layer: " << pCallbackData->pMessage << std::endl;

			return VK_FALSE;
		}
	}

	//--------------------------------------------------------------------------------------------------
	RenderContextVulkan::~RenderContextVulkan()
	{
		deinit();
	}

	//--------------------------------------------------------------------------------------------------
	void RenderContextVulkan::createWindow(
		math::Vec2i& position,
		math::Vec2u& size,
		const char* name,
		bool fullScreen,
		bool showCursor)
	{
		SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);

		if(fullScreen)
		{
			size = { 
				(uint32_t)GetSystemMetrics(SM_CXSCREEN),
				(uint32_t)GetSystemMetrics(SM_CYSCREEN) };
		}

		auto wnd = rev::gfx::createWindow(
			position, size,
			name,
			fullScreen, showCursor, true);
		m_windowSize = size;
		m_nativeWindowHandle = wnd;

		// Hook up window resizing callback
		*core::OSHandler::get() += [&](MSG _msg) {
			if (_msg.message == WM_SIZING || _msg.message == WM_SIZE)
			{
				// Get new rectangle size without borders
				RECT clientSurface;
				GetClientRect(_msg.hwnd, &clientSurface);
				m_windowSize = math::Vec2u(clientSurface.right, clientSurface.bottom);

				m_onResize(m_windowSize);
				return true;
			}
			if (_msg.message == WM_DPICHANGED)
			{
				const RECT* recommendedSurface = reinterpret_cast<const RECT*>(_msg.lParam);
				m_windowSize = math::Vec2u(
					recommendedSurface->right - recommendedSurface->left,
					recommendedSurface->bottom - recommendedSurface->top);
				SetWindowPos(wnd,
					NULL,
					recommendedSurface->left,
					recommendedSurface->top,
					m_windowSize.x(),
					m_windowSize.y(),
					SWP_NOZORDER | SWP_NOACTIVATE);

				m_onResize(m_windowSize);
				return true;
			}

			return false;
		};
	}

	//--------------------------------------------------------------------------------------------------
	bool RenderContextVulkan::initVulkan(
		const char* applicationName,
		uint32_t numAppDeviceExtensions, const char** appDeviceExtensionNames,
		uint32_t numLayers, const char** layerNames)
	{
		// Combine required extensions with application specific extensions
		constexpr size_t numRequiredExtensions = 1;
		const char* requiredExtensions[numRequiredExtensions] = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
		m_requiredDeviceExtensions.reserve(numAppDeviceExtensions + numRequiredExtensions);
		for(auto name : requiredExtensions)
			m_requiredDeviceExtensions.push_back(name);
		for (uint32_t i = 0; i < numAppDeviceExtensions; ++i)
		{
			auto& required = appDeviceExtensionNames[i];
			if(std::find(m_requiredDeviceExtensions.begin(), m_requiredDeviceExtensions.end(), required) == m_requiredDeviceExtensions.end())
				m_requiredDeviceExtensions.push_back(required);
		}

		// Store debug layers
		m_layers.reserve(numLayers);
		for(uint32_t i = 0; i < numLayers; ++i)
			m_layers.push_back(layerNames[i]);
#ifdef _DEBUG
		m_layers.push_back("VK_LAYER_KHRONOS_validation");
		m_layers.push_back("VK_LAYER_LUNARG_monitor");
#endif

		// Init API objects
		if(!createInstance(applicationName))
			return false;

		getPhysicalDevice();
		createLogicalDevice();

		return true;
	}

	bool RenderContextVulkan::createSwapchain()
	{
		if (!m_nativeWindowHandle)
			return false;
		
		initSurface();
		return initSwapChain();
	}

	//--------------------------------------------------------------------------------------------------
	bool RenderContextVulkan::createInstance(const char* applicationName)
	{
		const uint32_t engineVersion = VK_MAKE_VERSION(5, 0, 0);
		const uint32_t appVersion = VK_MAKE_VERSION(1, 0, 0);
		const uint32_t apiVersion = VK_MAKE_VERSION(1, 2, 0);
		vk::ApplicationInfo appInfo(applicationName, appVersion, "rev", engineVersion, apiVersion);

		// Technically, this is only necessary if we intend to create a surface, which we don't know yet.
		// In practice, these are available in all my test platforms so it's ok for now.
		constexpr auto numRequiredInstanceExtensions = 4;
		const char* requiredInstanceExtensions[numRequiredInstanceExtensions] = {
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
			VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME,
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME
		};

		vk::InstanceCreateInfo instanceInfo;
		// Query extensions
		m_properties.instanceExtensions = vk::enumerateInstanceExtensionProperties();

		// Check required extensions are available
		std::vector<std::string> available;
		available.reserve(m_properties.instanceExtensions.size());
		for(auto& extension : m_properties.instanceExtensions)
		{
			available.push_back(extension.extensionName);
		}
		for (auto& name : requiredInstanceExtensions) {
			if (std::find(available.begin(), available.end(), name) == available.end())
				return false;
		}
		instanceInfo.ppEnabledExtensionNames = requiredInstanceExtensions;
		instanceInfo.enabledExtensionCount = numRequiredInstanceExtensions;

		// Debug layers
		instanceInfo.enabledLayerCount = 0;
#ifdef _DEBUG
		constexpr uint32_t numLayers = 2;
		const char* instanceLayerNames[numLayers] = {
			"VK_LAYER_KHRONOS_validation",
			"VK_LAYER_LUNARG_monitor"
		};
		instanceInfo.enabledLayerCount = numLayers;
		instanceInfo.ppEnabledLayerNames = instanceLayerNames;
#endif
		m_vkInstance = vk::createInstance(instanceInfo);
		assert(m_vkInstance);

#ifdef _DEBUG
		// Debug callback
		vk::DebugUtilsMessengerCreateInfoEXT createInfo({},
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo,
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
			debugCallback, nullptr);
#endif
		return true;
	}

	//--------------------------------------------------------------------------------------------------
	void RenderContextVulkan::getPhysicalDevice()
	{
		auto physicalDevices = m_vkInstance.enumeratePhysicalDevices();
		vk::DeviceSize maxDiscreteMemorySize = 0;
		for(auto& device : physicalDevices)
		{
			if(!isDeviceSuitable(device))
				continue;

			auto memorySize = device.getMemoryProperties().memoryHeaps[0].size;
			if (memorySize > maxDiscreteMemorySize)
			{
				m_physicalDevice = device;
				maxDiscreteMemorySize = memorySize;
			}
		}

		assert(m_physicalDevice);
		m_queueFamilies = getDeviceQueueFamilies(m_physicalDevice);
	}

	//--------------------------------------------------------------------------------------------------
	void RenderContextVulkan::initSurface()
	{
		auto surfaceInfo = vk::Win32SurfaceCreateInfoKHR({}, GetModuleHandle(nullptr), m_nativeWindowHandle);
		m_surface = m_vkInstance.createWin32SurfaceKHR(surfaceInfo);
		assert(m_surface);
	}

	//--------------------------------------------------------------------------------------------------
	bool RenderContextVulkan::initSwapChain()
	{
		// sRGB support here
		auto surfaceFormats = m_physicalDevice.getSurfaceFormatsKHR(m_surface);
		auto targetFormat = vk::Format::eR8G8B8A8Srgb;
		if (std::find(surfaceFormats.begin(), surfaceFormats.end(), targetFormat) == surfaceFormats.end())
			return false;
		m_swapchain.imageFormat = targetFormat;

		// Check VSync support here
		auto presentModes = m_physicalDevice.getSurfacePresentModesKHR(m_surface);
		auto targetPresentMode = vk::PresentModeKHR::eFifo;
		if (std::find(presentModes.begin(), presentModes.end(), targetPresentMode) == presentModes.end())
			return false;

		// Must check device support before attempting to create the surface
		if (!m_physicalDevice.getSurfaceSupportKHR(m_queueFamilies.present.value(), m_surface))
			return false;

		vk::Extent2D imageSize { m_windowSize.x(), m_windowSize.y() };
		uint32_t minImageCount = 2; // Double buffered
		auto queueFamilyIndices = { m_queueFamilies.graphics.value() };
		auto surfaceInfo = vk::SwapchainCreateInfoKHR({},
			m_surface,
			minImageCount,
			targetFormat,
			vk::ColorSpaceKHR::eSrgbNonlinear,
			imageSize,
			1,
			vk::ImageUsageFlagBits::eColorAttachment,
			vk::SharingMode::eExclusive,
			queueFamilyIndices);
		
		m_swapchain.vkSwapchain = m_device.createSwapchainKHR(surfaceInfo);
		m_swapchain.images = m_device.getSwapchainImagesKHR(m_swapchain.vkSwapchain);

		m_swapchain.imageViews.reserve(m_swapchain.images.size());
		for (auto image : m_swapchain.images) {
			vk::ImageViewCreateInfo viewInfo({},
				image,
				vk::ImageViewType::e2D,
				m_swapchain.imageFormat,
				{ vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity },
				vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
			m_swapchain.imageViews.push_back(m_device.createImageView(viewInfo));
		}

		return true;
	}

	//--------------------------------------------------------------------------------------------------
	void RenderContextVulkan::createLogicalDevice()
	{
		constexpr float TopPriority = 1.f;
		// Create one graphics queue with full priority
		auto graphicsQueues = vk::DeviceQueueCreateInfo({}, m_queueFamilies.graphics.value(), 1, &TopPriority);
		// TODO: Add dedicated queues for transfer and async compute, etc (maybe more than one)
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfo = { graphicsQueues };

		// Specify required extensions
		vk::DeviceCreateInfo deviceInfo({}, queueCreateInfo, m_layers, m_requiredDeviceExtensions);
		m_device = m_physicalDevice.createDevice(deviceInfo);
		assert(m_device);

		// Retrieve command queues
		m_gfxQueue = m_device.getQueue(m_queueFamilies.graphics.value(), 0);
		assert(m_gfxQueue);
	}

	//--------------------------------------------------------------------------------------------------
	void RenderContextVulkan::deinit()
	{
		if(m_swapchain.images.size() > 0)
		{
			for (auto view : m_swapchain.imageViews)
			{
				m_device.destroyImageView(view);
			}
			m_device.destroySwapchainKHR(m_swapchain.vkSwapchain);
		}

		if(m_surface)
		{
			m_vkInstance.destroySurfaceKHR(m_surface);
		}

		m_device.destroy();
		// Destroy debug messenger
		if (m_debugMessenger)
		{
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(m_vkInstance, "vkDestroyDebugUtilsMessengerEXT");
			assert(func);
			func(m_vkInstance, m_debugMessenger, nullptr);
		}
		m_vkInstance.destroy();
	}

	//--------------------------------------------------------------------------------------------------
	bool RenderContextVulkan::isDeviceSuitable(const vk::PhysicalDevice& device)
	{
		// Check for required extensions
		std::vector<std::string> available;
		{
			auto deviceExtensions = device.enumerateDeviceExtensionProperties();
			available.reserve(deviceExtensions.size());
			for (auto extension : deviceExtensions)
			{
				available.push_back(extension.extensionName);
			}
		}

		for(auto& requiredExt : m_requiredDeviceExtensions)
		{
			if (std::find(available.begin(), available.end(), requiredExt) == available.end())
				return false;
		}

		// Queue families
		return getDeviceQueueFamilies(device).isFull();
	}

	//--------------------------------------------------------------------------------------------------
	auto RenderContextVulkan::getDeviceQueueFamilies(const vk::PhysicalDevice& device) -> QueueFamilies
	{
		QueueFamilies result;
		auto deviceFamilies = device.getQueueFamilyProperties();
		for (uint32_t i = 0; i < deviceFamilies.size(); ++i)
		{
			auto& family = deviceFamilies[i];
			if (family.queueFlags & vk::QueueFlagBits::eGraphics)
				result.graphics = i;
			if (family.queueFlags & vk::QueueFlagBits::eCompute)
				result.compute = i;
			if (family.queueFlags & vk::QueueFlagBits::eTransfer)
				result.transfer = i;
		}

		// Ideally, we would check for present capabilities of the device here. However,
		// that requires a specific surface to check against, and we don't have that yet.
		result.present = result.graphics;

		return result;
	}
}