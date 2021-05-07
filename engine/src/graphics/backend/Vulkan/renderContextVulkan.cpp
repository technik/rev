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

#define USE_VULKAN_LAYERS

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
		const char* requiredDeviceExtensions[] = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			// The following are a workaround for NSight to stop complaining about extensions that are part of Vulkan 1.2 anyway.
			VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME,
			VK_KHR_8BIT_STORAGE_EXTENSION_NAME,
			VK_KHR_16BIT_STORAGE_EXTENSION_NAME,
		};
		constexpr size_t numRequiredDeviceExtensions = sizeof(requiredDeviceExtensions) / sizeof(char*);
		m_requiredDeviceExtensions.reserve(numAppDeviceExtensions + numRequiredDeviceExtensions);
		for(auto name : requiredDeviceExtensions)
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
#ifdef USE_VULKAN_LAYERS
		m_layers.push_back("VK_LAYER_KHRONOS_validation");
		m_layers.push_back("VK_LAYER_LUNARG_monitor");
#endif

		// Init API objects
		if(!createInstance(applicationName))
			return false;

		getPhysicalDevice();
		createLogicalDevice();

		// Init vulkan allocator
		m_alloc = VulkanAllocator(m_device, m_physicalDevice, m_vkInstance, m_transferQueue, m_queueFamilies.transfer.value());

		return true;
	}

	bool RenderContextVulkan::createSwapchain(bool vSync)
	{
		if (!m_nativeWindowHandle)
			return false;
		
		initSurface();
		return initSwapChain(vSync);
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
		const char* requiredInstanceExtensions[] = {
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
			VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME,
			VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, // Workaround for NSight
#ifdef _DEBUG
			VK_EXT_DEBUG_REPORT_EXTENSION_NAME, // Workaround for NSight
#endif
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME
		};
		constexpr auto numRequiredInstanceExtensions = sizeof(requiredInstanceExtensions)/sizeof(char*);

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
#ifdef USE_VULKAN_LAYERS
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

#ifdef USE_VULKAN_LAYERS
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
	bool RenderContextVulkan::SwapChainInfo::init(
		vk::Device device,
		vk::SurfaceKHR surface,
		vk::Format imgFormat,
		const math::Vec2u& imageSize,
		vk::PresentModeKHR presentMode,
		uint32_t presentQueueFamily)
	{
		m_device = device;
		m_surface = surface;
		m_imageFormat = imgFormat;
		m_presentMode = presentMode;
		m_presentFamily = presentQueueFamily;

		// Create swapchain's render pass desc
		vk::AttachmentDescription colorAttachment;
		colorAttachment.initialLayout = vk::ImageLayout::eGeneral;
		colorAttachment.finalLayout = vk::ImageLayout::eGeneral;
		colorAttachment.format = imgFormat; // TODO: Should be swapchain's format?
		colorAttachment.samples = vk::SampleCountFlagBits::e1;
		colorAttachment.loadOp = vk::AttachmentLoadOp::eDontCare;
		colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
		vk::AttachmentReference colorAttachReference;
		colorAttachReference.attachment = 0;
		colorAttachReference.layout = vk::ImageLayout::eGeneral;
		vk::SubpassDescription colorSubpass;
		colorSubpass.colorAttachmentCount = 1;
		colorSubpass.pColorAttachments = &colorAttachReference;
		colorSubpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		auto renderPassInfo = vk::RenderPassCreateInfo({}, colorAttachment, colorSubpass);

		createSwapchain(imageSize);
		createImageViews();

		return true;
	}

	//--------------------------------------------------------------------------------------------------
	void RenderContextVulkan::SwapChainInfo::resize(const math::Vec2u& imageSize)
	{
		// Destroy image views
		for (auto view : imageViews)
			m_device.destroyImageView(view);
		imageViews.clear();
		// Destroy the swapchain
		m_device.destroySwapchainKHR(vkSwapchain);

		createSwapchain(imageSize);
		createImageViews();
	}

	//--------------------------------------------------------------------------------------------------
	void RenderContextVulkan::SwapChainInfo::createImageViews()
	{
		assert(imageViews.empty());
		imageViews.reserve(images.size());
		for (auto image : images) {
			vk::ImageViewCreateInfo viewInfo({},
				image,
				vk::ImageViewType::e2D,
				m_imageFormat,
				{ vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity },
				vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
			imageViews.push_back(m_device.createImageView(viewInfo));
		}
	}

	//--------------------------------------------------------------------------------------------------
	bool RenderContextVulkan::SwapChainInfo::createSwapchain(const math::Vec2u& imageSize)
	{
		const uint32_t minImageCount = 2; // Double buffered
		auto surfaceInfo = vk::SwapchainCreateInfoKHR({},
			m_surface,
			minImageCount,
			m_imageFormat,
			vk::ColorSpaceKHR::eSrgbNonlinear,
			vk::Extent2D{ imageSize.x(), imageSize.y() },
			1,
			vk::ImageUsageFlagBits::eColorAttachment
			| vk::ImageUsageFlagBits::eTransferDst, // So it can be cleared and copied to
			vk::SharingMode::eExclusive,
			m_presentFamily,
			vk::SurfaceTransformFlagBitsKHR::eIdentity,
			vk::CompositeAlphaFlagBitsKHR::eOpaque,
			m_presentMode);

		vkSwapchain = m_device.createSwapchainKHR(surfaceInfo);
		images = m_device.getSwapchainImagesKHR(vkSwapchain);
		return true;
	}

	//--------------------------------------------------------------------------------------------------
	bool RenderContextVulkan::initSwapChain(bool vSync)
	{
		// sRGB support here
		auto surfaceFormats = m_physicalDevice.getSurfaceFormatsKHR(m_surface);
		auto targetFormat = vk::Format::eR8G8B8A8Srgb;

		if (std::find(surfaceFormats.begin(), surfaceFormats.end(), targetFormat) == surfaceFormats.end())
		{
			// Try BGRA version, because some monitors are just weird
			targetFormat = vk::Format::eB8G8R8A8Srgb;
			if(std::find(surfaceFormats.begin(), surfaceFormats.end(), targetFormat) == surfaceFormats.end())
				return false;
		}

		// Check VSync support here
		auto presentModes = m_physicalDevice.getSurfacePresentModesKHR(m_surface);
		auto targetPresentMode = vSync ? vk::PresentModeKHR::eFifo : vk::PresentModeKHR::eImmediate;
		if (std::find(presentModes.begin(), presentModes.end(), targetPresentMode) == presentModes.end())
			return false;

		// Must check device support before attempting to create the surface
		if (!m_physicalDevice.getSurfaceSupportKHR(m_queueFamilies.present.value(), m_surface))
			return false;

		m_swapchain.init(m_device, m_surface, targetFormat, m_windowSize, targetPresentMode, m_queueFamilies.present.value());

		// Create render sync semaphores
		m_renderFinishedSemaphore = m_device.createSemaphore({});
		m_presentLayoutSemaphore = m_device.createSemaphore({});

		// Prepare per frame data
		m_frameData.reserve(m_swapchain.images.size());
		for (auto& img : m_swapchain.images)
		{
			m_frameData.emplace_back(m_device, m_queueFamilies.present.value());
		}

		return true;
	}

	//--------------------------------------------------------------------------------------------------
	void RenderContextVulkan::createLogicalDevice()
	{
		constexpr float TopPriority = 1.f;
		constexpr float LowPriority = 0.f;
		// Create one graphics queue with full priority
		auto graphicsQueues = vk::DeviceQueueCreateInfo({}, m_queueFamilies.graphics.value(), 1, &TopPriority);
		auto transferQueues = vk::DeviceQueueCreateInfo({}, m_queueFamilies.transfer.value(), 1, &LowPriority);
		// TODO: Add dedicated queues for transfer and async compute, etc (maybe more than one)
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfo = { graphicsQueues };
		if (m_queueFamilies.graphics.value() != m_queueFamilies.transfer.value())
			queueCreateInfo.push_back(transferQueues);

		// Specify required extensions
		vk::DeviceCreateInfo deviceInfo({}, queueCreateInfo, m_layers, m_requiredDeviceExtensions);
		m_device = m_physicalDevice.createDevice(deviceInfo);
		assert(m_device);

		// Retrieve command queues
		m_gfxQueue = m_device.getQueue(m_queueFamilies.graphics.value(), 0);
		m_transferQueue = m_device.getQueue(m_queueFamilies.transfer.value(), 0);
		assert(m_gfxQueue);
		assert(m_transferQueue);
	}

	//--------------------------------------------------------------------------------------------------
	void RenderContextVulkan::deinit()
	{
		m_frameData.clear(); // Free vulkan objects used per frame

		if(m_swapchain.images.size() > 0)
		{
			m_device.destroySemaphore(m_renderFinishedSemaphore);
			m_device.destroySemaphore(m_presentLayoutSemaphore);

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
			for (const auto& extension : deviceExtensions)
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
	
	//--------------------------------------------------------------------------------------------------
	vk::Image RenderContextVulkan::swapchainAquireNextImage(vk::Semaphore s, vk::CommandBuffer cmd)
	{
		auto res = m_device.acquireNextImageKHR(m_swapchain.vkSwapchain, uint64_t(-1), s);

		m_swapchain.frameIndex = res.value;
		auto nextImage = m_swapchain.currentImage();

		// Transition image to general layout before use
		auto clearRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
		vk::ImageMemoryBarrier presentToTransferBarrier(
			vk::AccessFlagBits::eColorAttachmentRead,
			vk::AccessFlagBits::eTransferWrite | vk::AccessFlagBits::eShaderWrite,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::eGeneral,
			m_queueFamilies.present.value(),
			m_queueFamilies.graphics.value(),
			nextImage,
			clearRange);

		cmd.pipelineBarrier(
			// Wait for
			vk::PipelineStageFlagBits::eColorAttachmentOutput,
			// Before
			vk::PipelineStageFlagBits::eTransfer // Clear or blit
			| vk::PipelineStageFlagBits::eFragmentShader // Raster
			| vk::PipelineStageFlagBits::eComputeShader, // Compute
			vk::DependencyFlagBits::eViewLocal,
			0, nullptr, // Memory barriers
			0, nullptr, // Buffer memory barriers
			1, &presentToTransferBarrier);

		return nextImage;
	}

	//--------------------------------------------------------------------------------------------------
	void RenderContextVulkan::swapchainPresent()
	{
		auto cmd = getNewRenderCmdBuffer();
		// TODO: Maybe can just record this command once on creation and reuse it.
		cmd.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

		// Transition swapchain image layout to presentable
		auto imageRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
		vk::ImageMemoryBarrier transferToPresentBarrier(
			vk::AccessFlagBits::eTransferWrite | vk::AccessFlagBits::eShaderWrite,
			vk::AccessFlagBits::eColorAttachmentRead,
			vk::ImageLayout::eGeneral,
			vk::ImageLayout::ePresentSrcKHR,
			m_queueFamilies.graphics.value(),
			m_queueFamilies.present.value(),
			m_swapchain.currentImage(),
			imageRange);
		cmd.pipelineBarrier(
			// Wait for
			vk::PipelineStageFlagBits::eTransfer // Clear or blit
			| vk::PipelineStageFlagBits::eFragmentShader // Raster
			| vk::PipelineStageFlagBits::eComputeShader, // Compute
			// Before
			vk::PipelineStageFlagBits::eColorAttachmentOutput,
			vk::DependencyFlagBits::eViewLocal,
			0, nullptr, // Memory barriers
			0, nullptr, // Buffer memory barriers
			1, &transferToPresentBarrier);

		// Submit render finished sync fence and semaphore
		cmd.end();
		vk::PipelineStageFlags waitFlags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		vk::SubmitInfo submitInfo(
			1, &m_renderFinishedSemaphore, &waitFlags, // wait
			1, &cmd, // commands
			1, &m_presentLayoutSemaphore); // signal
		m_gfxQueue.submit(submitInfo, m_frameData[m_frameDataNdx].renderFence);

		// Present image 
		auto presentInfo = vk::PresentInfoKHR(
			1, &m_presentLayoutSemaphore, // Wait on this
			1, &m_swapchain.vkSwapchain,
			&m_swapchain.frameIndex);

		auto res = m_gfxQueue.presentKHR(presentInfo);
		assert(res == vk::Result::eSuccess || res == vk::Result::eSuboptimalKHR);

		// Prepare next frame data for use
		m_frameDataNdx++;
		m_frameDataNdx %= m_frameData.size();
		m_frameData[m_frameDataNdx].reset();
	}

	//--------------------------------------------------------------------------------------------------
	void RenderContextVulkan::resizeSwapchain(const math::Vec2u& imageSize)
	{
		m_device.waitIdle();
		m_swapchain.resize(imageSize);
	}

	//--------------------------------------------------------------------------------------------------
	vk::CommandBuffer RenderContextVulkan::getNewRenderCmdBuffer()
	{
		return m_frameData[m_frameDataNdx].getRenderCmdBuffer();
	}

	//--------------------------------------------------------------------------------------------------
	RenderContextVulkan::FrameInfo::FrameInfo(vk::Device device, uint32_t gfxQueueFamily)
		: m_device(device)
	{
		// Create a command pool
		renderCommandPool = device.createCommandPool(vk::CommandPoolCreateInfo(
			vk::CommandPoolCreateFlagBits::eTransient | vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			gfxQueueFamily));

		// Allocate at least two command buffer for the frame
		// One for the application, one for signaling end of render synchornization
		vk::CommandBufferAllocateInfo cmdBufferInfo(renderCommandPool, vk::CommandBufferLevel::ePrimary, 2);
		renderCmdBuffers = device.allocateCommandBuffers(cmdBufferInfo);

		// Create a synchronization fence so we don't start writing commands while the buffers are still in flight
		renderFence = device.createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
	}

	//--------------------------------------------------------------------------------------------------
	RenderContextVulkan::FrameInfo::~FrameInfo()
	{
		m_device.destroyFence(renderFence);
		m_device.freeCommandBuffers(renderCommandPool, renderCmdBuffers);
		m_device.destroyCommandPool(renderCommandPool);
	}

	//--------------------------------------------------------------------------------------------------
	vk::CommandBuffer RenderContextVulkan::FrameInfo::getRenderCmdBuffer()
	{
		// First cmd buffer in the frame? wait for fence
		if (!usedBuffers)
		{
			const auto res = m_device.waitForFences(renderFence, 1, uint64_t(-1));
			assert(res == vk::Result::eSuccess);
			m_device.resetFences(renderFence);
		}

		if (usedBuffers == renderCmdBuffers.size()) // Exausted, allocate a new one
		{
			// Allocate at least one command buffer for the frame
			vk::CommandBufferAllocateInfo cmdBufferInfo(renderCommandPool, vk::CommandBufferLevel::ePrimary, 1);
			renderCmdBuffers.push_back(m_device.allocateCommandBuffers(cmdBufferInfo).front());
		}

		return renderCmdBuffers[usedBuffers++];
	}

	//--------------------------------------------------------------------------------------------------
	void RenderContextVulkan::FrameInfo::reset()
	{
		usedBuffers = 0;
	}
}