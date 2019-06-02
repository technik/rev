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

namespace rev::gfx
{
	enum class ScalarType : uint8_t
	{
		int8 = 1,
		uint8 = 1,
		int16 = 2,
		uint16 = 2,
		int32 = 4,
		uint32 = 4,
		float32 = 4,
	};

	struct PixelFormat
	{
		ScalarType componentType;
		std::uint8_t numChannels;
		bool sRGB = false;

		uint8_t pixelSize() const {
			return numChannels * uint8_t(componentType);
		}

		bool operator==(const PixelFormat& b) const
		{
			return componentType == b.componentType && numChannels == b.numChannels;
		}
	};

	enum class DescriptorType
	{
		RenderTarget,
		DepthStencil,
		Sampler,
		ShaderResource,
	};

	class DescriptorHeap
	{};

	class GpuBuffer
	{};

	enum class RenderTargetType
	{
		Depth,
		Color
	};

	class RenderTargetView
	{
	};

	enum BufferType
	{
		Resident, // Resident on gpu memory, fastest gpu bandwidth
		Upload, // Used to upload content from the CPU to the GPU
		ReadBack // Used to read content from the GPU back into the CPU
	};

	enum ResourceFlags
	{
		None = 0,
		IsRenderTarget = 1,
		IsDepthStencil = 2,
	};

	class CommandPool
	{
	public:
		enum Type
		{
			Graphics,
			Compute,
			Copy,
			Bundle
		};

		virtual void reset() = 0;
	};
}