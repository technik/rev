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
#include <graphics/backend/gpuTypes.h>

namespace rev :: gfx
{
	class Pipeline
	{
	public:
		using Id = int32_t;
		static constexpr Id InvalidId = -1;

		enum DataType
		{
			Float,
			Int32,
			U8,
		};

		struct Uniform
		{
			DataType componentType;
			uint32_t numComponents = 1;// 1 single, 2 vec2, 3 vec3, 4 vec4, 16 mat4
			uint32_t count = 0; // Array size, 0 if it's not an array

			uint32_t byteSize() const
			{
				uint32_t size = componentType == U8? 1 : 4;
				return size * numComponents * (count ? count : 1);
			}
		};

		struct UniformLayoutDesc
		{
			uint32_t numUniforms = 0;
			Uniform* uniform = nullptr;
		};

		struct Attribute
		{
			// Source buffer
			size_t binding; // Location (in shader)
			size_t offset = 0;
			DataType componentType = Float;
			size_t numComponents = 1;
			size_t stride = 0;
			bool normalized = false;
		};

		struct PipielineDesc
		{
			// Vtx stage
			int numAttributes;
			Attribute* vtxAttributes; // TODO: This can be reused later for instance data
			UniformLayoutDesc vtxUniforms;
			std::vector<std::string> vtxCode;

			// Pxl stage
			UniformLayoutDesc pxlUniforms;
			std::vector<std::string> pxlCode;
			RasterOptions raster;

			// TODO: Define render targets
		};
	};
}
