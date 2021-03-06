//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2021 Carmelo J Fdez-Aguera
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
#include "rasterPipeline.h"

#include <fstream>
#include <iostream>

#include <core/platform/fileSystem/fileSystem.h>
#include <core/platform/fileSystem/file.h>
#include <core/tools/log.h>

#include <gfx/types.h>

namespace rev::gfx
{
	RasterPipeline::RasterPipeline(
		const VertexBindings& vtxFormat,
		vk::PipelineLayout layout,
		vk::RenderPass passDesc,
		std::string_view vtxShaderFilename,
		std::string_view pxlShaderFilename,
		bool depthTest,
		bool blend
	)
		: m_vtxShader(vtxShaderFilename)
		, m_pxlShader(pxlShaderFilename)
		, m_attributes(vtxFormat.m_attributes)
		, m_layout(layout)
		, m_passDesc(passDesc)
		, m_depthTest(depthTest)
		, m_blend(blend)
	{
		m_vkPipeline = tryLoad();
	}

	RasterPipeline::~RasterPipeline()
	{
		clearPipeline();
	}

	void RasterPipeline::bind(const vk::CommandBuffer& cmdBuf)
	{
		if (m_invalidated)
		{
			if (reload())
				m_invalidated = false;
		}
		cmdBuf.bindPipeline(vk::PipelineBindPoint::eGraphics, m_vkPipeline);
	}

	void RasterPipeline::bindDescriptorSets(
			const vk::CommandBuffer& cmdBuf,
			const vk::ArrayProxy<const vk::DescriptorSet>& descSets,
			uint32_t firstSet)
	{}

	bool RasterPipeline::reload()
	{
		vk::Pipeline newPipeline = tryLoad();
		if (newPipeline)
		{
			RenderContext().device().waitIdle();
			clearPipeline();
			m_vkPipeline = newPipeline;
			return true;
		}

		return false;
	}

	void RasterPipeline::clearPipeline()
	{
		if (m_vkPipeline)
			RenderContext().device().destroyPipeline(m_vkPipeline);
	}

	vk::Pipeline RasterPipeline::tryLoad()
	{
		vk::ShaderModule vtxModule = loadShaderModule(m_vtxShader);
		if (!vtxModule)
		{
			core::LogMessage("Unable to load shader " + m_vtxShader);
			return vk::Pipeline();
		}
		vk::ShaderModule pxlModule = loadShaderModule(m_pxlShader);
		if (!pxlModule)
		{
			core::LogMessage("Unable to load shader " + m_pxlShader);
			return vk::Pipeline();
		}

		// Create shader stages
		auto vtxStage = vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, vtxModule, "main");
		auto pxlStage = vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, pxlModule, "main");
		vk::PipelineShaderStageCreateInfo stages[2] = { vtxStage, pxlStage };

		// Vertex input format
		std::vector<vk::VertexInputBindingDescription> vtxBindings;
		std::vector<vk::VertexInputAttributeDescription> vtxAttributes;
		for (auto& shaderAttribute : m_attributes)
		{
			auto& binding = vtxBindings.emplace_back();
			binding.binding = shaderAttribute.first;
			binding.stride = GetPixelSize(shaderAttribute.second);
			binding.inputRate = vk::VertexInputRate::eVertex;

			auto& attribute = vtxAttributes.emplace_back();
			attribute.binding = shaderAttribute.first;
			attribute.location = shaderAttribute.first;
			attribute.format = shaderAttribute.second;
			attribute.offset = 0;
		}

		vk::PipelineVertexInputStateCreateInfo vertexInputFormat({},
			vtxBindings, // Vertex bindings
			vtxAttributes // Vertex attributes
		);

		// Input assembly
		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo({}, vk::PrimitiveTopology::eTriangleList);

		// Dummy viewport, will be ignored because we mark it as dynamic state
		vk::Viewport dummyViewport;
		dummyViewport.height = 32;
		dummyViewport.width = 32;
		dummyViewport.x = 0;
		dummyViewport.y = 0;
		dummyViewport.maxDepth = 1.f;
		dummyViewport.minDepth = 0.f;
		vk::Rect2D dummyScissor({ 0,0 }, { 32, 32 });
		vk::PipelineViewportStateCreateInfo viewportInfo({},
			1, &dummyViewport,
			1, &dummyScissor);

		// Raster options
		auto rasterInfo = vk::PipelineRasterizationStateCreateInfo(
			{}, // Flags
			0, // No depth clamp
			0, // Don't discard rasterizer
			vk::PolygonMode::eFill,
			vk::CullModeFlagBits::eNone, // Cull back facing geometry
			vk::FrontFace::eCounterClockwise); // Front face direction

		// Multisampling
		auto multiSamplingInfo = vk::PipelineMultisampleStateCreateInfo();

		// Color blending
		vk::PipelineColorBlendAttachmentState colorBlend;
		colorBlend.colorWriteMask =
			vk::ColorComponentFlagBits::eR |
			vk::ColorComponentFlagBits::eG |
			vk::ColorComponentFlagBits::eB |
			vk::ColorComponentFlagBits::eA;
		colorBlend.blendEnable = m_blend;

		vk::PipelineColorBlendStateCreateInfo blendingInfo({},
			VK_FALSE,
			vk::LogicOp::eNoOp,
			1, &colorBlend,
			{ 1.f, 1.f, 1.f, 1.f });

		vk::DynamicState dynamicStates[] =
		{
			vk::DynamicState::eScissor,
			vk::DynamicState::eViewport,
			vk::DynamicState::eLineWidth
		};
		vk::PipelineDynamicStateCreateInfo dynamicStateInfo({}, 3, dynamicStates);

		vk::PipelineDepthStencilStateCreateInfo depthInfo;
		depthInfo.depthTestEnable = m_depthTest;
		depthInfo.depthWriteEnable = m_depthTest;
		depthInfo.depthCompareOp = vk::CompareOp::eGreater;

		// Pipeline info
		auto pipelineInfo = vk::GraphicsPipelineCreateInfo(
			{}, // Flags
			2, stages, // Shader stages
			&vertexInputFormat,
			&inputAssemblyInfo,
			nullptr, // Tesselation
			&viewportInfo, // Viewport (dynamic)
			&rasterInfo,
			&multiSamplingInfo,
			&depthInfo);
		pipelineInfo.setLayout(m_layout);
		pipelineInfo.setRenderPass(m_passDesc);
		pipelineInfo.setPDynamicState(&dynamicStateInfo);
		pipelineInfo.setPColorBlendState(&blendingInfo);

		auto device = RenderContext().device();
		auto newPipeline = device.createGraphicsPipeline({}, pipelineInfo);

		// Clean up
		device.destroyShaderModule(vtxModule);
		device.destroyShaderModule(pxlModule);

		if (newPipeline.result != vk::Result::eSuccess)
		{
			std::cout << "Error loading pipeline " << m_vtxShader << ", "<< m_pxlShader << ".\n";
		}
		return newPipeline.value;
	}

	vk::ShaderModule RasterPipeline::loadShaderModule(const std::string& fileName)
	{
		// Load shader file
		auto spirvFile = core::FileSystem::get()->readFile(fileName);
		if (!spirvFile)
			return vk::ShaderModule(); // Invalid by default

		size_t size = spirvFile->size();

		// Compile spirv
		vk::ShaderModuleCreateInfo moduleInfo({}, spirvFile->size(), spirvFile->buffer<uint32_t>());
		return RenderContext().device().createShaderModule(moduleInfo);
	}
}
