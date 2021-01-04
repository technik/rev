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

namespace rev :: gfx
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

		const math::Vec2u& windowSize() const { return m_windowSize; }
		auto& onResize() { return m_onResize; };

		// Vulkan
		bool initVulkan(
			const char* applicationName,
			uint32_t numAppDeviceExtensions = 0, const char** appDeviceExtensionNames = nullptr,
			uint32_t numLayers = 0, const char** layerNames = nullptr);

		// Swapchain
		bool createSwapchain(bool vSync);
		auto nSwapChainImages() const { return m_swapchain.images.size(); }
		vk::Image currentSwapChainImage() const;
		void swapchainAquireNextImage(vk::Semaphore dst);
		void swapchainPresent(vk::Semaphore src);

		// Device
		auto device() const { return m_device; }
		auto graphicsQueue() const { return m_gfxQueue; }
		auto graphicsQueueFamily() const { return m_queueFamilies.graphics.value(); }

		// Alloc
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
			vk::SwapchainKHR vkSwapchain;
			vk::Format imageFormat;
			std::vector<vk::Image> images;
			std::vector<vk::ImageView> imageViews;
			uint32_t frameIndex;

			auto currentImage() const { return images[frameIndex]; }
		} m_swapchain;

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

		// Debug
		vk::DebugUtilsMessengerEXT m_debugMessenger;
	};
}
