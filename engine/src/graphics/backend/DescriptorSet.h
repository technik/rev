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
#pragma once

#include <graphics/backend/Vulkan/renderContextVulkan.h>

#include <map>

namespace rev::gfx
{
	class DescriptorSetLayout
	{
	public:
		void addStorageBuffer(std::string name, uint32_t bindingPos, vk::ShaderStageFlags shaderStages)
		{
			m_bindings.emplace_back(bindingPos, vk::DescriptorType::eStorageBuffer, 1, shaderStages);
			if (!name.empty())
				m_namedBindings.insert({ name, bindingPos });
			++m_numStorageBuffers;
		}

		void addTexture(std::string name, uint32_t bindingPos, vk::ShaderStageFlags shaderStages)
		{
			m_bindings.emplace_back(bindingPos, vk::DescriptorType::eCombinedImageSampler, 1, shaderStages);
			if (!name.empty())
				m_namedBindings.insert({ name, bindingPos });
			++m_numTextures;
		}

		void addTextureArray(std::string name, uint32_t bindingPos, uint32_t numTextures, vk::ShaderStageFlags shaderStages)
		{
			m_bindings.emplace_back(bindingPos, vk::DescriptorType::eCombinedImageSampler, numTextures, shaderStages);
			if (!name.empty())
				m_namedBindings.insert({ name, bindingPos });
			m_numTextures += numTextures;
		}

		void close(uint32_t poolSize)
		{
			assert(!m_vkLayout);
			vk::DescriptorSetLayoutCreateInfo setLayoutInfo({}, m_bindings);

			auto device = RenderContext().device();
			m_vkLayout = device.createDescriptorSetLayout(setLayoutInfo);

			createDescriptorPool(poolSize);
			createDescriptors(poolSize);
		}

		vk::DescriptorSet getDescriptor(uint32_t index) const
		{
			return m_descriptorSets[index];
		}

		auto layout() const { return m_vkLayout; }
	private:

		void createDescriptorPool(size_t numDescriptorSets)
		{
			auto device = RenderContext().device();

			const uint32_t numDescriptorsPerSet = m_numTextures + m_numStorageBuffers;
			const uint32_t numDescriptorBindings = numDescriptorsPerSet * numDescriptorSets;
			vk::DescriptorPoolSize poolSize[2] = {
				{vk::DescriptorType::eStorageBuffer, numDescriptorSets * m_numStorageBuffers},
				{vk::DescriptorType::eCombinedImageSampler, numDescriptorSets * m_numTextures }
			};

			auto poolInfo = vk::DescriptorPoolCreateInfo({}, numDescriptorBindings);
			poolInfo.pPoolSizes = poolSize;
			poolInfo.poolSizeCount = 2;

			m_pool = device.createDescriptorPool(poolInfo);
		}

		void createDescriptors(size_t numDescriptors)
		{
			auto device = RenderContext().device();

			std::vector<vk::DescriptorSetLayout> setLayouts(numDescriptors, m_vkLayout);
			vk::DescriptorSetAllocateInfo setInfo(m_pool, setLayouts);
			m_descriptorSets = device.allocateDescriptorSets(setInfo);
		}

		std::vector<vk::DescriptorSetLayoutBinding> m_bindings;
		std::map<std::string, uint32_t> m_namedBindings;
		uint32_t m_numTextures = 0;
		uint32_t m_numStorageBuffers = 0;
		vk::DescriptorSetLayout m_vkLayout;
		vk::DescriptorPool m_pool;
		std::vector<vk::DescriptorSet> m_descriptorSets;
	};

	struct DescriptorSetUpdate
	{
		void add();
		void send();
	};
}