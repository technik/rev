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
#pragma once

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>
#include <vector>
#include <optional>

#include <core/event.h>
#include <math/algebra/vector.h>
#include "vulkanAllocator.h"
#include "../ScopedCommandBuffer.h"

namespace rev::gfx
{
	class RenderContextVulkan
	{
	public:
		RenderContextVulkan() = default;
		~RenderContextVulkan();

		// Window
		// If full screen is enabled, size and position will be updated to
		// the actual screen resolution and position used
		void createWindow(
			math::Vec2i& position,
			math::Vec2u& size,
			const char* name,
			bool fullScreen,
			bool showCursor);

		auto nativeWindow() const { return m_nativeWindowHandle; }
		const math::Vec2u& windowSize() const { return m_windowSize; }
		auto& onResize() { return m_onResize; };
		using ResizeDelegate = std::shared_ptr<core::Event<math::Vec2u>::Listener>;

		// Vulkan
		bool initVulkan(
			const char* applicationName,
			uint32_t numAppDeviceExtensions = 0, const char** appDeviceExtensionNames = nullptr,
			uint32_t numLayers = 0, const char** layerNames = nullptr);

		// Swapchain
		bool createSwapchain(bool vSync);
		void resizeSwapchain(const math::Vec2u& imageSize);
		auto swapchainImageView(size_t i) const { return m_swapchain.imageViews[i]; }
		auto nSwapChainImages() const { return m_swapchain.images.size(); }
		vk::Image swapchainAquireNextImage(vk::Semaphore signal, vk::CommandBuffer cmd);
		const vk::Semaphore& readyToPresentSemaphore() const { return m_renderFinishedSemaphore; }
		auto currentFrameIndex() const { return m_swapchain.frameIndex; }
		void swapchainPresent();
		vk::Format swapchainFormat() const { return m_swapchain.m_imageFormat; }

		// Device, Queues and Commands
		auto device() const { return m_device; }
		auto physicalDevice() const { return m_physicalDevice; }
		auto instance() const { return m_vkInstance; }
		auto graphicsQueue() const { return m_gfxQueue; }
		auto graphicsQueueFamily() const { return m_queueFamilies.graphics.value(); }
		auto transferQueue() const { return m_transferQueue; }
		vk::CommandBuffer getNewRenderCmdBuffer();
		ScopedCommandBuffer getScopedCmdBuffer(vk::Queue submitQueue);

		// Render passes and frame buffers
		vk::RenderPass createRenderPass(const std::vector<vk::Format>& attachmentFormats);
		void transitionImageLayout(vk::Image image, vk::Format imageFormat, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, bool isDepth);

		// Alloc
		VulkanAllocator& allocator() { return m_alloc; }
		// Debug
		// Device properties (capabilities)
		struct Properties
		{
			std::vector<vk::ExtensionProperties> instanceExtensions;
		};

		const Properties properties() const { m_properties; }

	private:
		bool createInstance(const char* applicationName);
		void getPhysicalDevice();
		void initSurface();
		bool initSwapChain(bool vSync);
		void createLogicalDevice();
		void deinit();

		bool isDeviceSuitable(const vk::PhysicalDevice&);

	private:
		// Window
		HWND m_nativeWindowHandle { NULL };
		core::Event<math::Vec2u> m_onResize;
		math::Vec2u m_windowSize { 0, 0 };

		// Device
		vk::Instance m_vkInstance;
		vk::PhysicalDevice m_physicalDevice;
		vk::Device m_device;
		Properties m_properties;
		std::vector<const char*> m_requiredDeviceExtensions;
		std::vector<const char*> m_layers;

		// Swapchain
		vk::SurfaceKHR m_surface;
		struct SwapChainInfo
		{
			vk::SurfaceKHR m_surface;
			vk::SwapchainKHR vkSwapchain;
			vk::Format m_imageFormat;
			std::vector<vk::Image> images;
			std::vector<vk::ImageView> imageViews;
			uint32_t frameIndex;

			auto currentImage() const { return images[frameIndex]; }

			bool init(
				vk::Device device,
				vk::SurfaceKHR surface,
				vk::Format imgFormat,
				const math::Vec2u& imageSize,
				vk::PresentModeKHR presentMode,
				uint32_t presentQueueFamily);

			void resize(const math::Vec2u& imageSize);

		private:
			bool createSwapchain(const math::Vec2u& imageSize);
			void createImageViews();

			uint32_t m_presentFamily;
			vk::PresentModeKHR m_presentMode;
			vk::Device m_device;
		} m_swapchain;
		vk::Semaphore m_renderFinishedSemaphore;
		vk::Semaphore m_presentLayoutSemaphore;

		// Queues
		struct QueueFamilies
		{
			std::optional<uint32_t> present;
			std::optional<uint32_t> graphics;
			std::optional<uint32_t> compute;
			std::optional<uint32_t> transfer;

			bool isFull() const {
				return present.has_value() &&
					graphics.has_value() &&
					compute.has_value() &&
					transfer.has_value();
			}
		} m_queueFamilies;
		QueueFamilies getDeviceQueueFamilies(const vk::PhysicalDevice& device);
		vk::Queue m_gfxQueue;
		vk::Queue m_transferQueue;

		// Commands
		struct FrameInfo
		{
			FrameInfo(vk::Device device, uint32_t gfxQueueFamily);
			~FrameInfo();

			vk::CommandBuffer getRenderCmdBuffer();
			void reset();

			vk::Fence renderFence;
		private:
			vk::CommandPool renderCommandPool;
			size_t usedBuffers{};

			std::vector<vk::CommandBuffer> renderCmdBuffers;
			vk::Device m_device;
		};
		std::vector<FrameInfo> m_frameData;
		size_t m_frameDataNdx{};

		// Debug
		vk::DebugUtilsMessengerEXT m_debugMessenger;

		// Allocator
		VulkanAllocator m_alloc;
	};
}
