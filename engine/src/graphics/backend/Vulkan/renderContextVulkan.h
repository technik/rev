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

#include <core/event.h>
#include <math/algebra/vector.h>

namespace rev :: gfx
{
	class RenderContextVulkan
	{
	public:
		RenderContextVulkan() = default;
		~RenderContextVulkan() = default;

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

		// Swapchain
		// Device
		// Alloc
		// Debug
		// Device properties (capabilities)

	private:
		HWND nativeWindowHandle { NULL };
		core::Event<math::Vec2u> m_onResize;
		math::Vec2u m_windowSize { 0, 0 };
	};
}
