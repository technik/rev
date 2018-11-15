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
#include "geometryPass.h"
#include <graphics/shaders/shaderCodeFragment.h>

namespace rev::gfx {

	//----------------------------------------------------------------------------------------------
	GeometryPass::GeometryPass(Device& device, ShaderCodeFragment& passCommonCode)
		: mPassCommonCode(&passCommonCode)
	{
		passCommonCode.onReload([this](const ShaderCodeFragment&)
		{
			mPipelines.clear();
		});
	}

	//----------------------------------------------------------------------------------------------
	void GeometryPass::render(
		const std::vector<RenderGeom*>& geometry,
		const std::vector<Instance>& instances,
		CommandBuffer& out)
	{
		// Render state caches
		RenderGeom* lastGeom = nullptr;
		ShaderCodeFragment* lastCode = nullptr;
		Pipeline::RasterOptions::Mask lastMask = 0;

		for (auto& instance : instances)
		{
			// Set up graphics pipeline
			if (lastCode != instance.instanceCode || lastMask != instance.raster)
			{
				lastCode = instance.instanceCode;
				lastMask = instance.raster;
				auto pipeline = getPipeline(instance);
				out.setPipeline(pipeline);
			}

			// Set up geometry
			auto geom = geometry[instance.geometryIndex];
			if (geom != lastGeom)
			{
				lastGeom = geom;
				out.setVertexData(geom->getVao());
			}

			// Set up uniforms
			out.setUniformData(instance.uniforms);
			// Draw
			CommandBuffer::IndexType indexType = CommandBuffer::IndexType::U16;
			if(geom->indices().componentType == GL_UNSIGNED_BYTE)
				CommandBuffer::IndexType indexType = CommandBuffer::IndexType::U8;
			if (geom->indices().componentType == GL_UNSIGNED_INT)
				CommandBuffer::IndexType indexType = CommandBuffer::IndexType::U32;
			out.drawTriangles(geom->indices().count, indexType);
		}
	}

}	// namespace rev::gfx