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
//#include "geometryPass.h"
#include <math/algebra/matrix.h>
#include <gfx/renderer/material/material.h>

using namespace rev::math;

namespace rev::gfx {

	/*//----------------------------------------------------------------------------------------------
	GeometryPass::GeometryPass(Device& device, ShaderCodeFragment* passCommonCode)
		: mDevice(device)
		, mPassCommonCode(passCommonCode)
	{
		m_shaderListeners.push_back(mPassCommonCode->onReload([this](const ShaderCodeFragment&)
		{
			m_pipelines.clear();
		}));

		// Common pipeline config
		m_commonPipelineDesc.raster.cullBack = true;
		m_commonPipelineDesc.raster.depthTest = Pipeline::DepthTest::Gequal;
	}

	//----------------------------------------------------------------------------------------------
	void GeometryPass::render(
		const Mat44f& view,
		const Mat44f& proj,
		const std::vector<RenderItem>& geometry,
		Pipeline::RasterOptions rasterOptions,
		Material::Flags bindingFlags,
		CommandBuffer& out)
	{
		auto worldMatrix = Mat44f::identity();
		GeometryPass::Instance instance;
		instance.geometryIndex = uint32_t(-1);
		instance.instanceCode = nullptr;

		std::vector<GeometryPass::Instance> renderList;
		std::vector<const RenderGeom*> geoms;
		const RenderGeom* lastGeom = nullptr;
		const Material* lastMaterial = nullptr;

		for (auto& mesh : geometry)
		{
			// Raster options
			bool mirroredGeometry = affineTransformDeterminant(worldMatrix) < 0.f;
			rasterOptions.frontFace = mirroredGeometry ? Pipeline::Winding::CW : Pipeline::Winding::CCW;
			instance.raster = rasterOptions.mask();
			// Uniforms
			instance.uniforms.clear();
			Mat44f world = mesh.world;
			Mat44f wvp = proj * (view * world); // world/view multiplied first for improved precision
			instance.uniforms.mat4s.push_back({ 0, wvp });
			instance.uniforms.mat4s.push_back({ 1, world });
			// Geometry
			if ((lastGeom != mesh.geom) || (lastMaterial != mesh.material))
			{
				lastMaterial = mesh.material;
				lastGeom = mesh.geom;
				mesh.material->bindParams(instance.uniforms, bindingFlags);
				instance.instanceCode = getMaterialCode(mesh.geom->vertexFormat(), *mesh.material);
				instance.geometryIndex++;
				geoms.push_back(mesh.geom);
			}
			renderList.push_back(instance);
		}
		
		render(geoms, renderList, out);
	}

	void GeometryPass::render(const std::vector<const RenderGeom*>& geometry,
		const std::vector<Instance>& renderList,
		CommandBuffer& out)
	{
		// Render state caches
		const RenderGeom* lastGeom = nullptr;
		ShaderCodeFragment* lastCode = nullptr;
		Pipeline::RasterOptions::Mask lastMask = 0;

		for (auto& instance : renderList)
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
		auto iter = m_pipelines.find(key);
		if(iter == m_pipelines.end())
		{
			// Extract code
			Pipeline::ShaderModule::Descriptor stageDesc;
			if (Pipeline::RasterOptions::fromMask(instance.raster).alphaMask)
				stageDesc.code.push_back("#define ALPHA_MASK\n");
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

			iter = m_pipelines.emplace(key, pipeline).first;
		}
		return iter->second;
	}

	ShaderCodeFragment* GeometryPass::getMaterialCode(VtxFormat vtxFormat, const Material& material)
	{
		auto completeCode = vtxFormat.shaderDefines() + material.bakedOptions() + material.effect().code();
		auto iter = m_materialCode.find(completeCode);
		if (iter == m_materialCode.end())
		{
			iter = m_materialCode.emplace(completeCode, new ShaderCodeFragment(completeCode.c_str())).first;
			material.effect().onReload([this](const Effect&) {
				m_materialCode.clear();
				});
		}
		return iter->second;
	}*/

}	// namespace rev::gfx