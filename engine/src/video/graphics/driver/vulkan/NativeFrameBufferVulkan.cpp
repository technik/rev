//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "NativeFrameBufferVulkan.h"
#include <video/window/window.h>
#include <iostream>
#include <vector>

using namespace std;

namespace rev { namespace video {

	namespace {	// anonymous namespace for vulkan utilities

		vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
			assert(!availableFormats.empty());
			// When format is undefined, it means we get to choose whatever format we want
			if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined) {
				return { vk::Format::eR8G8B8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
			}

			// Prefer rgba8 + sRGB
			for (const auto& availableFormat : availableFormats) {
				if (availableFormat.format == vk::Format::eR8G8B8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
					return availableFormat;
				}
			}

			// Prefer rgba8
			for (const auto& availableFormat : availableFormats) {
				if (availableFormat.format == vk::Format::eR8G8B8A8Unorm) {
					return availableFormat;
				}
			}

			// Return whatever we have
			return availableFormats[0];
		}

		vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes) {
			return vk::PresentModeKHR::eFifo;
		}
	}

	//--------------------------------------------------------------------------------------------------------------
	NativeFrameBufferVulkan::NativeFrameBufferVulkan(const Window& _wnd, vk::Instance _apiInstance, const VulkanDriver& _driver)
		: mApiInstance(_apiInstance)
		, mDevice(_driver.device())
	{
#ifdef _WIN32
		if(!initSurface(_wnd, _driver))
			return;
#else
		if(!initSurface())
			return;
#endif
		// Swap chain options
		auto support = _driver.querySwapChainSupport(mSurface);
		auto surfaceFormat = chooseSwapSurfaceFormat(support.formats);
		mImageFormat = surfaceFormat.format;
		auto presentMode = chooseSwapPresentMode(support.presentModes);
		auto bufferExtent = support.capabilities.currentExtent;
		mSize = { bufferExtent.width, bufferExtent.height };

		// Swap chain creation info
		auto createInfo = vk::SwapchainCreateInfoKHR({}, mSurface, 2, surfaceFormat.format, surfaceFormat.colorSpace, bufferExtent, 1,
			vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive, 0, nullptr, vk::SurfaceTransformFlagBitsKHR::eIdentity,
			vk::CompositeAlphaFlagBitsKHR::eOpaque, presentMode, VK_TRUE);

		// Create the swap chain
		if (mDevice.createSwapchainKHR(&createInfo, nullptr, &mSwapChain) != vk::Result::eSuccess) {
			cout << "failed to create swap chain!";
			return;
		}

		// Retrieve buffer image handles
		uint32_t imageCount;
		mDevice.getSwapchainImagesKHR(mSwapChain, &imageCount, nullptr);
		mSwapChainImages.resize(imageCount);
		mDevice.getSwapchainImagesKHR(mSwapChain, &imageCount, mSwapChainImages.data());

		// Create image views for the images
		mSwapChainImageViews.reserve(imageCount);
		for(auto image : mSwapChainImages) {
			auto createInfo = vk::ImageViewCreateInfo({}, image,
				vk::ImageViewType::e2D,
				surfaceFormat.format,
				vk::ComponentMapping(),
				vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));

			vk::ImageView view;
			if(mDevice.createImageView(&createInfo, nullptr, &view) != vk::Result::eSuccess) {
				cout << "failed to create image views within the native frame buffer\n";
			}
			mSwapChainImageViews.push_back(view);
		}

		

		// Set up attachment desc so this FB can be used in a render pass
		setupAttachmentDesc(); // Depends on FB configuration being ready
	}

	//--------------------------------------------------------------------------------------------------------------
	NativeFrameBufferVulkan::~NativeFrameBufferVulkan() {
		for(auto view : mSwapChainImageViews)
			mDevice.destroyImageView(view);
		mDevice.destroySwapchainKHR(mSwapChain);
		mApiInstance.destroySurfaceKHR(mSurface);
	}

	//--------------------------------------------------------------------------------------------------------------
#ifdef _WIN32
	bool NativeFrameBufferVulkan::initSurface(const Window& _wnd, const VulkanDriver& _driver) {
		auto createInfo = vk::Win32SurfaceCreateInfoKHR({}, GetModuleHandle(nullptr), _wnd.winapiHandle());

		// Get the extension
		auto CreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR) mApiInstance.getProcAddr("vkCreateWin32SurfaceKHR");

		// Create the surface
		if (!CreateWin32SurfaceKHR || mApiInstance.createWin32SurfaceKHR(&createInfo, nullptr, &mSurface) != vk::Result::eSuccess) {
			cout << "failed to create window surface!\n";
			return false;
		}

		VkBool32 presentSupport = false;
		vk::PhysicalDevice(_driver.physicalDevice()).getSurfaceSupportKHR(_driver.graphicsFamily(), mSurface, &presentSupport);

		return presentSupport;
	}
#endif // _WIN32
#ifdef ANDROID
	bool VulkanDriver::initSurface(VkInstance _apiInstance) {
		LOGI("Init surface");
		// Get display information
		//uint32_t displayPropCount = 0;
		//vkGetPhysicalDeviceDisplayPropertiesKHR(mPhysicalDevice, &displayPropCount, nullptr);

		// Get extension
		auto fpCreateAndroidSurfaceKHR = (PFN_vkCreateDisplayPlaneSurfaceKHR)vkGetInstanceProcAddr(_apiInstance, "vkCreateAndroidSurfaceKHR");
		if (fpCreateAndroidSurfaceKHR == nullptr) {
			//LOGE("Unable to get create surface extension");
			return false;
		}

		// Set display mode
		/*VkDisplayModeKHR

		VkDisplaySurfaceCreateInfoKHR	createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
		createInfo.*/
		LOGI("Finish Init surface");
		return true;
	}
#endif // ANDROID

	//--------------------------------------------------------------------------------------------------------------
	void NativeFrameBufferVulkan::setupAttachmentDesc() {
		mAttachDesc = vk::AttachmentDescription({}, mImageFormat,
			vk::SampleCountFlagBits::e1, // No multisampling
			vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore, // Color and depth persistency
			vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, // Stencil persistency
			vk::ImageLayout::eUndefined, // Don't care, we're clearing it
			vk::ImageLayout::ePresentSrcKHR); // Prepare to present, this goes directly to a swapchain
	}
}}