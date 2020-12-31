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

namespace rev::gfx {

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
	void RenderContextVulkan::initVulkan(const char* applicationName)
	{
		createInstance(applicationName);
	}

	//--------------------------------------------------------------------------------------------------
	void RenderContextVulkan::createInstance(const char* applicationName)
	{
		const uint32_t engineVersion = VK_MAKE_VERSION(5, 0, 0);
		const uint32_t appVersion = VK_MAKE_VERSION(1, 0, 0);
		const uint32_t apiVersion = VK_MAKE_VERSION(1, 2, 0);
		vk::ApplicationInfo appInfo(applicationName, appVersion, "rev", engineVersion, apiVersion);

		constexpr auto numRequiredExtensions = 1;
		const char* requiredExtensions[numRequiredExtensions] = {};

		vk::InstanceCreateInfo instanceInfo;
		constexpr uint32_t numLayers = 1;
		const char* instanceLayerNames[numLayers] = {
			"VK_LAYER_LUNARG_monitor"
		};
		instanceInfo.enabledLayerCount = numLayers;
		instanceInfo.ppEnabledLayerNames = instanceLayerNames;

		// Query extensions
		// Query available extensions count
		vkEnumerateInstanceExtensionProperties(nullptr, &instanceInfo.enabledExtensionCount, nullptr);
		
		// Allocate space for extension names
		m_properties.extensions.resize(instanceInfo.enabledExtensionCount);
		std::vector<char*> extensionNames;
		extensionNames.reserve(instanceInfo.enabledExtensionCount);
		vk::enumerateInstanceExtensionProperties(nullptr, &instanceInfo.enabledExtensionCount, m_properties.extensions.data());

		// Copy extension names into createInfo
		// TODO: Is it worth enabling only the extensions we actually intend to use?
		// TODO: Validate required extensions are available
		for (auto& extension : m_properties.extensions) {
			extensionNames.push_back(extension.extensionName);
		}
		instanceInfo.ppEnabledExtensionNames = extensionNames.data();

		m_vkInstance = vk::createInstance(instanceInfo);
		assert(m_vkInstance);
	}

	//--------------------------------------------------------------------------------------------------
	void RenderContextVulkan::deinit()
	{}
}