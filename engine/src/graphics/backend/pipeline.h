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
			struct Descriptor
			{
				enum Stage
				{
					Vertex,
					Pixel
				} stage;
				std::vector<std::string> code;
			};
			Id id = InvalidId;
		};

		// Vertex data / instance data
		struct Binding // Source buffers
		{
			// Binding index
			Id id = InvalidId;
		};

		struct Attribute
		{
			// Source buffer
			size_t location; // Location (in shader)
			size_t offset = 0;
			enum ComponentType
			{
				Float,
				Int32,
				U8,
			} componentType = Float;
			size_t nComponents = 1;
			size_t stride = 0;
			bool normalized = false;
		};

		// TODO: Uniform buffers (VkPipelineLayout)

		struct Descriptor {
			ShaderModule vtxShader;
			ShaderModule pxlShader;

			std::vector<Binding> vtxBuffers;
			std::vector<Binding> instanceBuffers;
			std::vector<Attribute> attributes;

			// TODO: Fixed function
			enum class DepthTest {
				None,
				Lequal,
				Gequal
			} depthTest = DepthTest::None;// Depth test
			// Blending
			bool cullBack = false;// Culling
			bool cullFront = false;
			enum Winding
			{
				CW,
				CCW
			} frontFace = CCW;

			// TODO: Uniform buffers
		};

		bool isValid() const { return id != InvalidId; }

		Id id = InvalidId;
	};
}
