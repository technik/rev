//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2019 Carmelo J Fdez-Aguera
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
// DirectX 12 specific headers.
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

#include "../graphicsDriver.h"
#include "../Windows/windowsPlatform.h"

#include <wrl.h>

namespace rev :: gfx
{
	/// Handles low level DX12 tasks, like retrieving properties of physical devices
	class DirectX12Driver : public GraphicsDriver
	{
	public:
		DirectX12Driver();

		// Physical devices
		int enumeratePhysicalDevices(PhysicalDeviceInfo* dst, int maxDevices) override;
		PhysicalDevice* createPhysicalDevice(int deviceIndex) override;
		Device* createDevice(const PhysicalDevice& adapter) override;

	private:
		struct PhysicalDeviceDX12 : PhysicalDevice
		{
			ComPtr<IDXGIAdapter4> m_adapter;
		};

	private:
		void createDeviceFactory();
		Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
	};
}
#pragma once
