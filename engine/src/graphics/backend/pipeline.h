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

			bool valid() const { return id != InvalidId; }
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

		enum class DepthTest {
			None = 0,
			Lequal = 1,
			Gequal = 2,
			Less = 4,
		};

		enum class BlendMode
		{
			Write,
			Additive
		};

		enum class Winding
		{
			CW,
			CCW
		};

		struct RasterOptions
		{
			bool cullBack = false;// Culling
			bool cullFront = false;
			Winding frontFace = Winding::CCW;
			DepthTest depthTest = DepthTest::None;// Depth test
			BlendMode blendMode = BlendMode::Write;
			bool witeDepth = true;

			using Mask = uint32_t;

			Mask mask() const {
				Mask m = 0;
				m |= (cullBack?1:0);
				m |= (cullFront?1:0)<<1;
				m |= ((frontFace==Winding::CCW)?1:0)<<2;
				m |= (witeDepth ? 1 : 0) << 3;
				m |= (int(depthTest))<<4;
				return m;
			}

			static RasterOptions fromMask(Mask m)
			{
				RasterOptions r;
				r.cullBack = m & 1;
				r.cullFront = m & (1<<1);
				r.frontFace = Winding((m>>2) & 1);
				r.witeDepth = m & (1 << 3);
				r.depthTest = DepthTest(m>>4);
				return r;
			}
		};
		// TODO: Uniform buffers (VkPipelineLayout)

		struct Descriptor {
			ShaderModule vtxShader;
			ShaderModule pxlShader;

			std::vector<Binding> vtxBuffers;
			std::vector<Binding> instanceBuffers;
			std::vector<Attribute> attributes;

			RasterOptions raster;

			// TODO: Blending
			// TODO: Uniform buffers
		};

		bool isValid() const { return id != InvalidId; }

		Id id = InvalidId;
	};
}
