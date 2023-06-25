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

#include "CommandQueue.h"
#include "core/types.h"

namespace rev::gfx
{
	struct UAVHandleTag;
	struct SRVHandleTag;
	struct RTVHandleTag;
	struct GPUResourceHandleTag;

	using UAV = core::TypedHandle<UAVHandleTag>;
	using SRV = core::TypedHandle<SRVHandleTag>;
	using RTV = core::TypedHandle<RTVHandleTag>;
	using GPUResource = core::TypedHandle<GPUResourceHandleTag>;

	class ImageResource
	{
	public:
		// Internal resource views
		UAV uav;
		RTV rtv;
		SRV srv;

		GPUResource resource;
	};

	// Entry point to the graphics API
	class Device
	{
	public:
		// Graphics API
		enum class GfxAPI
		{
			DX12,
			Vulkan
		};

		virtual ~Device() {};

		virtual CommandQueue* createCommandQueue(CommandQueue::Info) = 0;
	};

} // namespace rev::gfx