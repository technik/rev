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
#pragma once

#include <cassert>
#include <string>

#include <core/event.h>
#include <math/algebra/vector.h>

namespace rev::gfx
{
	class Device;
	class CommandQueue;

	// A graphics context is an API independent utility for grouping low level graphics infrastructure
	class Context
	{
	public:
		// Graphics API
		enum class GfxAPI
		{
			DX12,
			Vulkan
		};

		struct SwapChainOptions
		{
			bool vSync;
			bool hdr;
			bool fullScreen;
			// refresh rate?
		};

		// Window management
		// If full screen is enabled, size and position will be updated to
		// the actual screen resolution and position used
		bool createWindow(
			math::Vec2i& position,
			math::Vec2u& size,
			const char* name,
			bool showCursor,
			SwapChainOptions& swapChainDesc);

		void destroyWindow() {} // TODO

		auto nativeWindow() const { return m_nativeWindowHandle; }
		const math::Vec2u& windowSize() const { return m_windowSize; }
		auto& onResize() { return m_onResize; };
		using ResizeDelegate = std::shared_ptr<core::Event<math::Vec2u>::Listener>;

		// Device, Queues and Commands
		auto& device() const { return *m_device; }

		virtual CommandQueue& GfxQueue() const = 0;
		virtual CommandQueue& AsyncComputeQueue() const = 0;
		virtual CommandQueue& CopyQueue() const = 0;

		// Singleton
		static bool initSingleton(const char* applicationName, GfxAPI backendAPI = GfxAPI::Vulkan);
		static void endSingleton();

		__forceinline static Context* singleton() { return s_instance; }

		struct DeviceInfo
		{
			std::string name;
			size_t dediactedVideoMemory = 0;
			bool vSyncOffSupport = false;
		};

		const DeviceInfo& deviceInfo() const { return m_deviceInfo; }

	protected:
		virtual void end() = 0;

		virtual ~Context()
		{
			assert(s_instance == this);
			s_instance = nullptr;
		}

		Context()
		{
			assert(!s_instance);
		}

		virtual bool createSwapChain(const SwapChainOptions&, const math::Vec2u& imageSize) = 0;
		virtual math::Vec2u resizeSwapChain(const math::Vec2u& imageSize) = 0; // Returns the actual size the swapchain was resized to
		virtual void destroySwapChain() = 0;

		// API device
		Device* m_device = nullptr;
		DeviceInfo m_deviceInfo;

	private:

		Context(const Context&) = delete;
		Context& operator=(const Context&) = delete;

		// Singleton
		inline static Context* s_instance = nullptr;

		// Window management
		void* m_nativeWindowHandle {}; // HWND is internally a regular pointer, so we use one here to avoid including windows headers in an engine header
		core::Event<math::Vec2u> m_onResize;
		math::Vec2u m_windowSize{ 0, 0 };
	};

	__forceinline Context& RenderContext()
	{
		assert(Context::singleton());
		return *Context::singleton();
	}

} // namespace rev::gfx