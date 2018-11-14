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

#include <graphics/backend/frameBuffer.h>
#include <graphics/backend/commandBuffer.h>
#include <graphics/scene/renderGeom.h>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace rev::gfx {

	class Device;
	class ShaderCodeFragment;

	class GeometryPass
	{
	public:
		GeometryPass(Device& device, ShaderCodeFragment& passCommonCode);

		// PipelineGroupId can be filled with arbitrary user content.
		// Usually, they're a bitmask representing things like shadow use,
		// mirrored geometry, etc.
		// The important thing is that two pieces of geometry with different
		// PipelineGroupId will always have separate shader pipelines
		using PipelineGroupId = uint32_t;

		struct Instance
		{
			uint32_t geometryIndex;
			uint32_t extraNdx;
			ShaderCodeFragment* instanceCode;
			Pipeline::RasterOptions::Mask raster;
			CommandBuffer::UniformBucket uniforms;
		};

		// Processes the suplied geometry and uniforms, and stores the generated commands into out.
		void begin();
		void render(
			const std::vector<RenderGeom*>& geometry,
			const std::vector<std::string>& extraCodeBlocks,
			const std::vector<Instance>& instances,
			CommandBuffer& out);

	private:

		ShaderCodeFragment* mPassCommonCode; // Effect containing the pass' common code

		using PipelineSrc = std::pair<Pipeline::RasterOptions::Mask, ShaderCodeFragment*>;
		// Stored pipelines
		std::unordered_map<PipelineSrc, Pipeline> mPipelines;
	};

}	// namespace rev::gfx