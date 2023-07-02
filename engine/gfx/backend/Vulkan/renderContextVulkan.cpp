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
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "renderContextVulkan.h"
#include "vulkanCommandQueue.h"
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
		s_instance = nullptr;
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
		m_alloc = VulkanAllocator(m_vkDevice, m_physicalDevice, m_vkInstance, m_transferQueue->nativeQueue(), m_queueFamilies.transfer.value());

		return true;
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

		m_deviceInfo.dediactedVideoMemory = maxDiscreteMemorySize;
	}

	//--------------------------------------------------------------------------------------------------
	void RenderContextVulkan::initSurface()
	{
		auto surfaceInfo = vk::Win32SurfaceCreateInfoKHR({}, GetModuleHandle(nullptr), (HWND)RenderContext().nativeWindow());
		m_surface = m_vkInstance.createWin32SurfaceKHR(surfaceInfo);

		// Check VSync support here
		m_deviceInfo.vSyncOffSupport = false;
		auto presentModes = m_physicalDevice.getSurfacePresentModesKHR(m_surface);
		if (std::find(presentModes.begin(), presentModes.end(), vk::PresentModeKHR::eImmediate) != presentModes.end())
		{
			m_deviceInfo.vSyncOffSupport = true;
		}
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

		createSwapchain(imageSize);

		return true;
	}

	//--------------------------------------------------------------------------------------------------
	void RenderContextVulkan::SwapChainInfo::resize(const math::Vec2u& imageSize)
	{
		// Destroy image views
		for (auto& buffer: m_imageBuffers)
			m_device.destroyImageView(buffer.view());
		m_imageBuffers.clear();
		// Destroy the swapchain
		m_device.destroySwapchainKHR(m_vkSwapchain);

		createSwapchain(imageSize);
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

		m_vkSwapchain = m_device.createSwapchainKHR(surfaceInfo);
		auto images = m_device.getSwapchainImagesKHR(m_vkSwapchain);

		assert(m_imageBuffers.empty());
		m_imageBuffers.reserve(images.size());
		for (auto image : images) {
			vk::ImageViewCreateInfo viewInfo({},
				image,
				vk::ImageViewType::e2D,
				m_imageFormat,
				{ vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity },
				vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
			auto view = m_device.createImageView(viewInfo);

			m_imageBuffers.emplace_back(image, view, m_imageFormat);
		}
		return true;
	}

	//--------------------------------------------------------------------------------------------------
	bool RenderContextVulkan::createSwapChain(const SwapChainOptions& swapChainDesc, const math::Vec2u& imageSize)
	{
		if (!m_surface)
		{
			initSurface();
		}

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

		// Must check device support before attempting to create the surface
		if (!m_physicalDevice.getSurfaceSupportKHR(m_queueFamilies.present.value(), m_surface))
			return false;

		auto targetPresentMode = swapChainDesc.vSync ? vk::PresentModeKHR::eFifo : vk::PresentModeKHR::eImmediate;
		m_swapchain.init(m_vkDevice, m_surface, targetFormat, RenderContext().windowSize(), targetPresentMode, m_queueFamilies.present.value());


		// Create render sync semaphores
		m_renderFinishedSemaphore = m_vkDevice.createSemaphore({});
		m_presentLayoutSemaphore = m_vkDevice.createSemaphore({});

		// Prepare per frame data
		m_frameData.reserve(m_swapchain.m_imageBuffers.size());
		for (auto& img : m_swapchain.m_imageBuffers)
		{
			m_frameData.emplace_back(m_vkDevice, m_queueFamilies.present.value());
		}

		return true;
	}

	//--------------------------------------------------------------------------------------------------
	void RenderContextVulkan::createLogicalDevice()
	{
		constexpr float TopPriority = 1.f;
		constexpr float AsyncPriority = 0.5f;
		constexpr float LowPriority = 0.f;
		// Create one graphics queue with full priority
		auto graphicsQueues = vk::DeviceQueueCreateInfo({}, m_queueFamilies.graphics.value(), 1, &TopPriority);
		auto asyncQueues = vk::DeviceQueueCreateInfo({}, m_queueFamilies.compute.value(), 1, &AsyncPriority);
		auto transferQueues = vk::DeviceQueueCreateInfo({}, m_queueFamilies.transfer.value(), 1, &LowPriority);
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfo = { graphicsQueues, asyncQueues, transferQueues };

		// Specify required extensions
		vk::DeviceCreateInfo deviceInfo({}, queueCreateInfo, m_layers, m_requiredDeviceExtensions);
		m_vkDevice = m_physicalDevice.createDevice(deviceInfo);
		assert(m_vkDevice);

		// Retrieve command queues
		// TODO: How do we know these are the right indices? indices into the descs? indices into each family?
		m_gfxQueue = new VulkanCommandQueue(m_vkDevice, m_queueFamilies.graphics.value());
		m_computeQueue = new VulkanCommandQueue(m_vkDevice, m_queueFamilies.compute.value());
		m_transferQueue = new VulkanCommandQueue(m_vkDevice, m_queueFamilies.transfer.value());
		assert(m_gfxQueue);
		assert(m_computeQueue);
		assert(m_transferQueue);
	}

	//--------------------------------------------------------------------------------------------------
	void RenderContextVulkan::deinit()
	{
		delete m_gfxQueue;
		delete m_computeQueue;
		delete m_transferQueue;

		m_frameData.clear(); // Free vulkan objects used per frame

		if(m_swapchain.m_imageBuffers.size() > 0)
		{
			m_vkDevice.destroySemaphore(m_renderFinishedSemaphore);
			m_vkDevice.destroySemaphore(m_presentLayoutSemaphore);

			for (auto& image : m_swapchain.m_imageBuffers)
			{
				m_vkDevice.destroyImageView(image.view());
			}
			m_vkDevice.destroySwapchainKHR(m_swapchain.m_vkSwapchain);
		}

		if(m_surface)
		{
			m_vkInstance.destroySurfaceKHR(m_surface);
		}

		m_vkDevice.destroy();
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
			{
				result.graphics = i;
				continue;
			}
			if (family.queueFlags & vk::QueueFlagBits::eCompute)
			{
				result.compute = i;
				continue;
			}
			if (family.queueFlags & vk::QueueFlagBits::eTransfer)
			{
				result.transfer = i;
			}
		}

		// Ideally, we would check for present capabilities of the device here. However,
		// that requires a specific surface to check against, and we don't have that yet.
		result.present = result.graphics;

		return result;
	}
	
	//--------------------------------------------------------------------------------------------------
	const ImageBuffer& RenderContextVulkan::swapchainAquireNextImage(vk::Semaphore s, vk::CommandBuffer cmd)
	{
		auto res = m_vkDevice.acquireNextImageKHR(m_swapchain.m_vkSwapchain, uint64_t(-1), s);

		m_swapchain.frameIndex = res.value;
		return m_swapchain.currentImage();
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
			m_swapchain.currentImage().image(),
			imageRange);
		cmd.pipelineBarrier(
			// Wait for
			vk::PipelineStageFlagBits::eTransfer // Clear or blit
			| vk::PipelineStageFlagBits::eFragmentShader // Raster
			| vk::PipelineStageFlagBits::eComputeShader, // Compute
			// Before
			vk::PipelineStageFlagBits::eColorAttachmentOutput,
			{},//vk::DependencyFlagBits::eViewLocal,
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

		auto queue = static_cast<VulkanCommandQueue&>(GfxQueue()).nativeQueue();
		
		queue.submit(submitInfo, m_frameData[m_frameDataNdx].renderFence);

		// Present image 
		auto presentInfo = vk::PresentInfoKHR(
			1, &m_presentLayoutSemaphore, // Wait on this
			1, &m_swapchain.m_vkSwapchain,
			&m_swapchain.frameIndex);

		auto res = queue.presentKHR(presentInfo);
		assert(res == vk::Result::eSuccess || res == vk::Result::eSuboptimalKHR);

		// Prepare next frame data for use
		m_frameDataNdx++;
		m_frameDataNdx %= m_frameData.size();
		m_frameData[m_frameDataNdx].reset();
	}

	//--------------------------------------------------------------------------------------------------
	math::Vec2u RenderContextVulkan::resizeSwapChain(const math::Vec2u& imageSize)
	{
		m_vkDevice.waitIdle();

		// Find out current device capabilities. Needed to move across different dpi surfaces
		vk::SurfaceCapabilitiesKHR capabilities;
		auto result = m_physicalDevice.getSurfaceCapabilitiesKHR(m_surface, &capabilities);
		assert(result == vk::Result::eSuccess);

		math::Vec2u clampedSize = {
			std::min(capabilities.maxImageExtent.width, std::max(capabilities.minImageExtent.width, imageSize.x())),
			std::min(capabilities.maxImageExtent.height, std::max(capabilities.minImageExtent.height, imageSize.y()))
		};

		m_swapchain.resize(clampedSize);
		// m_windowSize = clampedSize; // Should this go into the base context somewhere?

		return clampedSize;
	}

	//--------------------------------------------------------------------------------------------------
	vk::CommandBuffer RenderContextVulkan::getNewRenderCmdBuffer()
	{
		return m_frameData[m_frameDataNdx].getRenderCmdBuffer();
	}

	//--------------------------------------------------------------------------------------------------
	ScopedCommandBuffer RenderContextVulkan::getScopedCmdBuffer(vk::Queue submitQueue, vk::Semaphore waitForSemaphore)
	{
		return ScopedCommandBuffer(getNewRenderCmdBuffer(), submitQueue, waitForSemaphore);
	}

	//--------------------------------------------------------------------------------------------------
	vk::RenderPass RenderContextVulkan::createRenderPass(const std::vector<vk::Format>& attachmentFormats)
	{
		// UI Render pass
		std::vector<vk::AttachmentDescription> attachmentInfo;
		std::vector<vk::AttachmentReference> colorReferences;
		std::vector<vk::AttachmentReference> depthReferences;

		for (const auto& attachmentFormat : attachmentFormats)
		{
			auto& info = attachmentInfo.emplace_back();
			info.initialLayout = vk::ImageLayout::eGeneral;
			info.finalLayout = vk::ImageLayout::eGeneral;
			info.format = attachmentFormat;
			info.samples = vk::SampleCountFlagBits::e1;
			info.loadOp = vk::AttachmentLoadOp::eLoad;
			info.storeOp = vk::AttachmentStoreOp::eStore;

			vk::AttachmentReference reference;
			reference.attachment = uint32_t(attachmentInfo.size() - 1);
			reference.layout = vk::ImageLayout::eGeneral; // TODO: eColorOptimal/eDepthOptimal?

			if (info.format == vk::Format::eD32Sfloat)
				depthReferences.push_back(reference);
			else
				colorReferences.push_back(reference);
		}

		vk::SubpassDescription subpass;
		subpass.colorAttachmentCount = (uint32_t)colorReferences.size();
		subpass.pColorAttachments = colorReferences.data();
		subpass.pDepthStencilAttachment = depthReferences.empty() ? nullptr : depthReferences.data();
		subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;

		auto passInfo = vk::RenderPassCreateInfo({}, attachmentInfo, subpass);

		return m_vkDevice.createRenderPass(passInfo);
	}

	//--------------------------------------------------------------------------------------------------
	void RenderContextVulkan::transitionImageLayout(vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, bool isDepth)
	{
		auto cmd = getNewRenderCmdBuffer();
		cmd.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

		allocator().transitionImageLayout(cmd, image, oldLayout, newLayout, isDepth);

		cmd.end();
		vk::SubmitInfo submitInfo(
			0, nullptr, nullptr, // wait
			1, &cmd, // commands
			0, nullptr); // signal

		static_cast<VulkanCommandQueue&>(GfxQueue()).nativeQueue().submit(submitInfo);
	}

	//--------------------------------------------------------------------------------------------------
	CommandQueue& RenderContextVulkan::GfxQueue() const
	{
		return *m_gfxQueue;
	}

	//--------------------------------------------------------------------------------------------------
	CommandQueue& RenderContextVulkan::AsyncComputeQueue() const
	{
		return *m_computeQueue;
	}

	//--------------------------------------------------------------------------------------------------
	CommandQueue& RenderContextVulkan::CopyQueue() const
	{
		return *m_transferQueue;
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