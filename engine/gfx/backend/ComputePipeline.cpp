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
#include "ComputePipeline.h"

#include <fstream>
#include <iostream>

#include <core/platform/fileSystem/fileSystem.h>
#include <core/platform/fileSystem/file.h>
#include <core/tools/log.h>

#include <gfx/types.h>

namespace rev::gfx
{
	ComputePipeline::ComputePipeline(
		vk::PipelineLayout layout,
		std::string_view shaderFilename
	)
		: m_shader(shaderFilename)
		, m_layout(layout)
	{
		m_vkPipeline = tryLoad();
	}

	ComputePipeline::~ComputePipeline()
	{
		clearPipeline();
	}

	void ComputePipeline::bind(const vk::CommandBuffer& cmdBuf)
	{
		if (m_invalidated)
		{
			if (reload())
				m_invalidated = false;
		}
		cmdBuf.bindPipeline(vk::PipelineBindPoint::eGraphics, m_vkPipeline);
	}

	void ComputePipeline::bindDescriptorSets(
		const vk::CommandBuffer& cmdBuf,
		const vk::ArrayProxy<const vk::DescriptorSet>& descSets,
		uint32_t firstSet)
	{}

	bool ComputePipeline::reload()
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

	void ComputePipeline::clearPipeline()
	{
		if (m_vkPipeline)
			RenderContext().device().destroyPipeline(m_vkPipeline);
	}

	vk::Pipeline ComputePipeline::tryLoad()
	{
		vk::ShaderModule shaderModule = loadShaderModule(m_shader);
		if (!shaderModule)
		{
			core::LogMessage("Unable to load shader " + m_shader);
			return vk::Pipeline();
		}

		// Create shader stages
		auto shaderStage = vk::PipelineShaderStageCreateInfo({}, vk::ShaderStageFlagBits::eCompute, shaderModule, "main");

		// Pipeline info
		auto pipelineInfo = vk::ComputePipelineCreateInfo();
		pipelineInfo.layout = m_layout;
		pipelineInfo.setLayout(m_layout);
		pipelineInfo.setStage(shaderStage);

		auto device = RenderContext().device();
		auto newPipeline = device.createComputePipeline({}, pipelineInfo);

		// Clean up
		device.destroyShaderModule(shaderModule);

		if (newPipeline.result != vk::Result::eSuccess)
		{
			std::cout << "Error loading pipeline " << m_shader << ".\n";
		}
		return newPipeline.value;
	}

	vk::ShaderModule ComputePipeline::loadShaderModule(const std::string& fileName)
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
