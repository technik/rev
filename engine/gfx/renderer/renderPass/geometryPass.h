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

#include <gfx/scene/renderGeom.h>
#include <gfx/shaders/shaderCodeFragment.h>
#include <gfx/renderer/material/material.h>
#include <gfx/renderer/RenderItem.h>
#include <string>
#include <map>
#include <utility>
#include <vector>

namespace rev::gfx {

	/*class ShaderCodeFragment;

	class GeometryPass
	{
	public:
		GeometryPass(Device& device, ShaderCodeFragment* passCommonCode);

		struct Instance
		{
			ShaderCodeFragment* instanceCode;
			Pipeline::RasterOptions::Mask raster;
			CommandBuffer::UniformBucket uniforms;
			uint32_t geometryIndex;
		};

		// Processes the suplied geometry and uniforms, and stores the generated commands into out.
		void render(
			const math::Mat44f& view,
			const math::Mat44f& proj,
			const std::vector<RenderItem>& geometry,
			Pipeline::RasterOptions rasterOptions,
			Material::Flags bindingFlags,
			CommandBuffer& out);

		// Processes the suplied geometry and uniforms, and stores the generated commands into out.
		void render(
			const std::vector<const RenderGeom*>& geometry,
			const std::vector<Instance>& instances,
			CommandBuffer& out);

	private:

		ShaderCodeFragment* getMaterialCode(VtxFormat, const Material& material);

		Device& mDevice;
		Pipeline getPipeline(const Instance&);

		ShaderCodeFragment* mPassCommonCode; // Effect containing the pass' common code
		Pipeline::Descriptor m_commonPipelineDesc; // Config common to all shadow pipelines

		using PipelineSrc = std::pair<Pipeline::RasterOptions::Mask, ShaderCodeFragment*>;
		// Stored pipelines
		std::map<std::string, ShaderCodeFragment*> m_materialCode;
		std::map<PipelineSrc, Pipeline> m_pipelines;
		std::vector<std::shared_ptr<ShaderCodeFragment::ReloadListener>> m_shaderListeners;
	};*/

}	// namespace rev::gfx