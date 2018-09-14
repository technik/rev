//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2018 Carmelo J Fdez-Aguera
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
#include <cstdint>
#include <vector>

namespace rev :: gfx
{
	class Pipeline
	{
	public:
		using Id = int32_t;
		static constexpr Id InvalidId = -1;

		struct ShaderModule
		{
			Id id;
		};

		// Vertex data / instance data
		struct Binding // Source buffers
		{
			// Binding index

		};

		struct Attribute
		{
			// Source buffer
			// Location (in shader)
		};

		// TODO: Uniform buffers (VkPipelineLayout)

		struct Descriptor {
			ShaderModule vtxShader;
			ShaderModule pxlShader;

			std::vector<Binding> vtxBuffers;
			std::vector<Binding> instanceBuffers;
			std::vector<Attribute> attributes;

			// TODO: Fixed function
			// Depth test
			// Blending
			// Culling

			// TODO: Uniform buffers
		};

		Id id = InvalidId;
	};
}
