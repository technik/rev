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

namespace rev::gfx {

	//----------------------------------------------------------------------------------------------
	/*GeometryPass::GeometryPass(Device& device, ShaderCodeFragment* passCommonCode)
		: mDevice(device)
		, mPassCommonCode(passCommonCode)
	{
		m_shaderListeners.push_back(mPassCommonCode->onReload([this](const ShaderCodeFragment&)
		{
			mPipelines.clear();
		}));

		// Common pipeline config
		m_commonPipelineDesc.raster.cullBack = true;
		m_commonPipelineDesc.raster.depthTest = Pipeline::DepthTest::Lequal;
	}

	//----------------------------------------------------------------------------------------------
	void GeometryPass::render(
		const std::vector<const RenderGeom*>& geometry,
		const std::vector<Instance>& instances,
		CommandBuffer& out)
	{
		// Render state caches
		const RenderGeom* lastGeom = nullptr;
		ShaderCodeFragment* lastCode = nullptr;
		Pipeline::RasterOptions::Mask lastMask = 0;

		for (auto& instance : instances)
		{
			// Set up graphics pipeline
			if (lastCode != instance.instanceCode || lastMask != instance.raster)
			{
				auto pipeline = getPipeline(instance);
				if(pipeline.isValid())
				{
					lastCode = instance.instanceCode;
					lastMask = instance.raster;
					out.setPipeline(pipeline);
				}
				else
					continue;
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
				indexType = CommandBuffer::IndexType::U8;
			if (geom->indices().componentType == GL_UNSIGNED_INT)
				indexType = CommandBuffer::IndexType::U32;
			out.drawTriangles(geom->indices().count, indexType, geom->indices().offset);
		}
	}

	//----------------------------------------------------------------------------------------------
	Pipeline GeometryPass::getPipeline(const Instance& instance)
	{
		auto key = std::pair(instance.raster, instance.instanceCode);
		auto iter = mPipelines.find(key);
		if(iter == mPipelines.end())
		{
			// Extract code
			Pipeline::ShaderModule::Descriptor stageDesc;
			if(instance.instanceCode)
				instance.instanceCode->collapse(stageDesc.code);
			mPassCommonCode->collapse(stageDesc.code);

			// Build pipeline stages
			stageDesc.stage = Pipeline::ShaderModule::Descriptor::Vertex;
			m_commonPipelineDesc.vtxShader = mDevice.createShaderModule(stageDesc);
			stageDesc.stage = Pipeline::ShaderModule::Descriptor::Pixel;
			m_commonPipelineDesc.pxlShader = mDevice.createShaderModule(stageDesc);

			// Check against invalid pipeline code
			Pipeline pipeline;
			if(m_commonPipelineDesc.vtxShader.valid()
				&& m_commonPipelineDesc.pxlShader.valid())
			{
				m_commonPipelineDesc.raster = Pipeline::RasterOptions::fromMask(instance.raster);
				pipeline = mDevice.createPipeline(m_commonPipelineDesc);
			}

			iter = mPipelines.emplace(key, pipeline).first;
		}
		return iter->second;
	}*/

}	// namespace rev::gfx