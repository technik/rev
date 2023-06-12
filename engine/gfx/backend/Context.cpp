//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2023 Carmelo J Fdez-Aguera
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
#include "Context.h"

#include "Device.h"
#include "Windows/windowsPlatform.h"
#include <core/platform/osHandler.h>
#include "./Vulkan/renderContextVulkan.h"
#include "./DX12/ContextDX12.h"

#include <iostream>

namespace rev::gfx
{
    //------------------------------------------------------------------------------------------------------------------
    bool Context::createWindow(
        math::Vec2i& position,
        math::Vec2u& size,
        const char* name,
		bool showCursor,
		SwapChainOptions& swapChainDesc)
	{
		SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);

		if (swapChainDesc.fullScreen)
		{
			size = {
				(uint32_t)GetSystemMetrics(SM_CXSCREEN),
				(uint32_t)GetSystemMetrics(SM_CYSCREEN) };
		}

		auto wnd = rev::gfx::createWindow(
			position, size,
			name,
			swapChainDesc.fullScreen, showCursor, true);
		m_windowSize = size;
		m_nativeWindowHandle = wnd;

		// Hook up window resizing callback
		*core::OSHandler::get() += [wnd,this](MSG _msg) {
			if (_msg.message == WM_SIZING || _msg.message == WM_SIZE)
			{
				// Get new rectangle size without borders
				RECT clientSurface;
				GetClientRect(_msg.hwnd, &clientSurface);
				m_windowSize = math::Vec2u(clientSurface.right, clientSurface.bottom);

				m_windowSize = resizeSwapChain(m_windowSize);

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

				m_windowSize = resizeSwapChain(m_windowSize);

				m_onResize(m_windowSize);
				return true;
			}

			return false;
		};

		return createSwapChain(swapChainDesc, m_windowSize);
	}

    //------------------------------------------------------------------------------------------------------------------
    bool Context::initSingleton(const char* appName, GfxAPI backendAPI)
    {
        // init the singleton
        assert(!s_instance); // Trying to initialize multiple times

        // Create the graphics device
        switch (backendAPI)
        {
        case GfxAPI::DX12:
        {
			auto context12 = new ContextDX12();
			if (context12->init(true))
			{
				s_instance = context12;
			}
			else
			{
				delete context12;
				return false;
			}
            break;
        }
        case GfxAPI::Vulkan:
        {
			auto vkContext = new RenderContextVulkan();
			if (vkContext->initVulkan(appName))
			{
				s_instance = vkContext;
			}
			else
			{
				delete vkContext;
				return false;
			}
            break;
        }
        }

		// Dump device info
		std::cout << "Using graphics adapter: " << s_instance->m_deviceInfo.name << "\n";

		size_t GB = s_instance->m_deviceInfo.dediactedVideoMemory >> 30;
		size_t MB = s_instance->m_deviceInfo.dediactedVideoMemory >> 20 & 0x3ff;

		std::cout << "Dedicated video memory:";
		if (GB > 0) std::cout << " " << GB << "GB";
		if (MB > 0) std::cout << " " << MB << "MB";
		std::cout << "\n";
		std::cout << "Supports unlocked framerate: " << s_instance->m_deviceInfo.vSyncOffSupport << "\n";

		return true;
    }

    //------------------------------------------------------------------------------------------------------------------
    void Context::endSingleton()
    {
		s_instance->end();

        delete s_instance->m_device;

        // clear the singleton
        assert(s_instance);
        delete s_instance;
        s_instance = nullptr;
    }
}