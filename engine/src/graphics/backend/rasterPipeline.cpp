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

namespace rev::gfx
{
	RasterPipeline::RasterPipeline(
		vk::Device device,
		vk::PipelineLayout layout,
		vk::RenderPass passDesc,
		std::string vtxShaderFilename,
		std::string pxlShaderFilename
	)
		: m_vtxShader(vtxShaderFilename)
		, m_pxlShader(pxlShaderFilename)
		, m_device(device)
		, m_layout(layout)
		, m_passDesc(passDesc)
	{
		tryLoad();
	}

	RasterPipeline::~RasterPipeline()
	{
	}

	void RasterPipeline::bind(const vk::CommandBuffer& cmdBuf)
	{
		cmdBuf.bindPipeline(vk::PipelineBindPoint::eGraphics, m_vkPipeline);
	}

	void RasterPipeline::bindDescriptorSets(
			const vk::CommandBuffer& cmdBuf,
			const vk::ArrayProxy<const vk::DescriptorSet>& descSets,
			uint32_t firstSet)
	{}

	template<class T>
	void RasterPipeline::pushConstant(const vk::CommandBuffer& cmdBuf, const T&)
	{}

	void RasterPipeline::tryLoad()
	{
		vk::ShaderModule vtxModule = loadShaderModule(m_vtxShader);
		vk::ShaderModule pxlModule = loadShaderModule(m_pxlShader);

		// Create shader stages
		auto vtxStage = vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, vtxModule, "main");
		auto pxlStage = vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, pxlModule, "main");
		vk::PipelineShaderStageCreateInfo stages[2] = { vtxStage, pxlStage };

		// Vertex input format
		vk::PipelineVertexInputStateCreateInfo vertexInputFormat({},
			{}, // Vertex bindings
			{} // Vertex attributes
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
			0, // Disable discard rasterizer
			vk::PolygonMode::eFill,
			vk::CullModeFlagBits::eBack, // Cull back facing geometry
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
		colorBlend.blendEnable = VK_FALSE;

		vk::PipelineColorBlendStateCreateInfo blendingInfo({},
			VK_FALSE,
			vk::LogicOp::eNoOp,
			1, &colorBlend,
			{ 1.f, 1.f, 1.f, 1.f});

		vk::DynamicState dynamicStates[] =
		{
			vk::DynamicState::eScissor,
			vk::DynamicState::eViewport,
			vk::DynamicState::eLineWidth
		};
		vk::PipelineDynamicStateCreateInfo dynamicStateInfo({}, 3, dynamicStates);

		// Pipeline info
		auto pipelineInfo = vk::GraphicsPipelineCreateInfo(
			{}, // Flags
			2, stages,
			&vertexInputFormat,
			&inputAssemblyInfo,
			nullptr, // Tesselation
			& viewportInfo, // Viewport (dynamic)
			&rasterInfo,
			&multiSamplingInfo,
			nullptr);
		pipelineInfo.setLayout(m_layout);
		pipelineInfo.setRenderPass(m_passDesc);
		pipelineInfo.setPDynamicState(&dynamicStateInfo);
		pipelineInfo.setPColorBlendState(&blendingInfo);

		auto newPipeline = m_device.createGraphicsPipeline({}, pipelineInfo);
		if (newPipeline.result == vk::Result::eSuccess)
		{
			m_vkPipeline = newPipeline.value;
		}

		// Clean up
		m_device.destroyShaderModule(vtxModule);
		m_device.destroyShaderModule(pxlModule);
	}

	vk::ShaderModule RasterPipeline::loadShaderModule(const std::string& fileName)
	{
		// Load shader file
		std::ifstream spirvFile(fileName, std::ios::binary | std::ios::ate);
		if (!spirvFile.is_open())
			return vk::ShaderModule(); // Invalid by default

		size_t size = spirvFile.tellg();
		std::vector<char> byteCode(size);
		spirvFile.seekg(std::ios::beg);
		spirvFile.read(byteCode.data(), size);

		// Compile spirv
		vk::ShaderModuleCreateInfo moduleInfo({}, size, (const uint32_t*)byteCode.data());
		return m_device.createShaderModule(moduleInfo);
	}
}
