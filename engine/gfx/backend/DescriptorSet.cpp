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
#include "DescriptorSet.h"
#include <gfx/backend/Vulkan/renderContextVulkan.h>

namespace rev::gfx
{
	void DescriptorSetLayout::addStorageBuffer(const std::string& name, uint32_t bindingPos, vk::ShaderStageFlags shaderStages)
	{
		m_bindings.emplace_back(bindingPos, vk::DescriptorType::eStorageBuffer, 1, shaderStages);
		assert(!name.empty());
		m_storageBufferBindings.insert({ name, bindingPos });
		++m_numStorageBuffers;
	}

	void DescriptorSetLayout::addTexture(const std::string& name, uint32_t bindingPos, vk::ShaderStageFlags shaderStages)
	{
		m_bindings.emplace_back(bindingPos, vk::DescriptorType::eCombinedImageSampler, 1, shaderStages);
		assert(!name.empty());
		m_textureBindings.insert({ name, bindingPos });
		++m_numTextures;
	}

	void DescriptorSetLayout::addImage(const std::string& name, uint32_t bindingPos, vk::ShaderStageFlags shaderStages)
	{
		m_bindings.emplace_back(bindingPos, vk::DescriptorType::eStorageImage, 1, shaderStages);
		assert(!name.empty());
		m_imageBindings.insert({ name, bindingPos });
		++m_numImages;
	}

	void DescriptorSetLayout::addTextureArray(const std::string& name, uint32_t bindingPos, uint32_t numTextures, vk::ShaderStageFlags shaderStages)
	{
		m_bindings.emplace_back(bindingPos, vk::DescriptorType::eCombinedImageSampler, numTextures, shaderStages);
		assert(!name.empty());
		m_textureArrayBindings.insert({ name, { bindingPos, numTextures } });
		m_numTextures += numTextures;
	}

	void DescriptorSetLayout::close()
	{
		assert(!m_vkLayout);
		vk::DescriptorSetLayoutCreateInfo setLayoutInfo({}, m_bindings);

		auto device = RenderContextVk().device();
		m_vkLayout = device.createDescriptorSetLayout(setLayoutInfo);
	}

	DescriptorSetPool::DescriptorSetPool(const std::shared_ptr<DescriptorSetLayout>& layout, uint32_t poolSize)
		: m_layout(layout)
	{
		createDescriptorPool(poolSize);
		createDescriptors(poolSize);
	}

	void DescriptorSetPool::createDescriptorPool(uint32_t numDescriptorSets)
	{
		const uint32_t numDescriptorsPerSet = m_layout->m_numTextures + m_layout->m_numStorageBuffers + m_layout->m_numImages;
		const uint32_t numDescriptorBindings = numDescriptorsPerSet * numDescriptorSets;
		std::vector<vk::DescriptorPoolSize> poolSize;
		if(m_layout->m_numStorageBuffers > 0)
		{
			poolSize.emplace_back(vk::DescriptorType::eStorageBuffer, numDescriptorSets * m_layout->m_numStorageBuffers);
		}
		if(m_layout->m_numTextures > 0)
		{
			poolSize.emplace_back(vk::DescriptorType::eCombinedImageSampler, numDescriptorSets * m_layout->m_numTextures);
		}
		if (m_layout->m_numImages > 0)
		{
			poolSize.emplace_back(vk::DescriptorType::eStorageImage, numDescriptorSets * m_layout->m_numImages);
		}

		auto poolInfo = vk::DescriptorPoolCreateInfo({}, numDescriptorBindings);
		poolInfo.pPoolSizes = poolSize.data();
		poolInfo.poolSizeCount = (uint32_t)poolSize.size();

		m_pool = RenderContextVk().device().createDescriptorPool(poolInfo);
	}

	void DescriptorSetPool::createDescriptors(uint32_t numDescriptors)
	{
		std::vector<vk::DescriptorSetLayout> setLayouts(numDescriptors, m_layout->m_vkLayout);
		vk::DescriptorSetAllocateInfo setInfo(m_pool, setLayouts);
		m_descriptorSets = RenderContextVk().device().allocateDescriptorSets(setInfo);
	}

	void DescriptorSetPool::writeArrayTextureToDescriptor(uint32_t descNdx, const std::string& name, const std::vector<std::shared_ptr<Texture>>& textureArray)
	{
		vk::WriteDescriptorSet writeInfo;
		writeInfo.dstSet = getDescriptor(descNdx);
		writeInfo.dstArrayElement = 0;

		assert(m_layout->m_textureArrayBindings.find(name) != m_layout->m_textureArrayBindings.end());
		auto [binding, numTextures] = m_layout->m_textureArrayBindings.at(name);
		
		if (textureArray.size() > 0)
		{
			std::vector<vk::DescriptorImageInfo> texInfo(textureArray.size());
			for (size_t i = 0; i < textureArray.size(); ++i)
			{
				auto& texture = *textureArray[i];
				texInfo[i].imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
				texInfo[i].imageView = texture.image->view();
				texInfo[i].sampler = texture.sampler;
			}
			writeInfo.descriptorType = vk::DescriptorType::eCombinedImageSampler;
			writeInfo.dstBinding = binding;
			writeInfo.descriptorCount = (uint32_t)textureArray.size();
			writeInfo.pImageInfo = texInfo.data();
			RenderContextVk().device().updateDescriptorSets(writeInfo, {});
		}
	}
	
	void DescriptorSetUpdate::addStorageBuffer(const std::string& name, std::shared_ptr<GPUBuffer> buffer)
	{
		// Locate binding in layout
		auto iter = m_pool.m_layout->m_storageBufferBindings.find(name);
		assert(iter != m_pool.m_layout->m_storageBufferBindings.end());

		m_bufferWrites.insert({ iter->second, buffer });
	}

	void DescriptorSetUpdate::addTexture(const std::string& name, std::shared_ptr<Texture> texture)
	{
		// Locate binding in layout
		auto iter = m_pool.m_layout->m_textureBindings.find(name);
		assert(iter != m_pool.m_layout->m_textureBindings.end());

		m_textureWrites.insert({ iter->second, texture });
	}

	void DescriptorSetUpdate::addImage(const std::string& name, std::shared_ptr<ImageBuffer> image)
	{
		// Locate binding in layout
		auto iter = m_pool.m_layout->m_imageBindings.find(name);
		assert(iter != m_pool.m_layout->m_imageBindings.end());

		m_imageWrites.insert({ iter->second, image });
	}

	void DescriptorSetUpdate::send() const
	{
		std::vector<vk::WriteDescriptorSet> writes;

		// Storage buffer writes
		std::vector<vk::DescriptorBufferInfo> bufferInfo;
		bufferInfo.reserve(m_bufferWrites.size());
		for (auto& [bindingPos, buffer] : m_bufferWrites)
		{
			auto& writeBufferInfo = bufferInfo.emplace_back(buffer->buffer(), 0, buffer->size());

			vk::WriteDescriptorSet& writeInfo = writes.emplace_back();
			writeInfo.dstSet = m_pool.getDescriptor(m_descNdx);
			writeInfo.descriptorType = vk::DescriptorType::eStorageBuffer;
			writeInfo.dstBinding = bindingPos;
			writeInfo.dstArrayElement = 0;
			writeInfo.descriptorCount = 1;
			writeInfo.pBufferInfo = &writeBufferInfo;
		}

		// Single texture writes
		std::vector<vk::DescriptorImageInfo> textureInfo;
		textureInfo.reserve(m_textureWrites.size());
		for (auto& [bindingPos, texture] : m_textureWrites)
		{
			vk::DescriptorImageInfo& writeTextureInfo = textureInfo.emplace_back();
			writeTextureInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			writeTextureInfo.imageView = texture->image->view();
			writeTextureInfo.sampler = texture->sampler;

			vk::WriteDescriptorSet& writeInfo = writes.emplace_back();
			writeInfo.dstSet = m_pool.getDescriptor(m_descNdx);
			writeInfo.descriptorType = vk::DescriptorType::eCombinedImageSampler;
			writeInfo.dstBinding = bindingPos;
			writeInfo.dstArrayElement = 0;
			writeInfo.descriptorCount = 1;
			writeInfo.pImageInfo = &writeTextureInfo;
		}

		// Single image writes
		std::vector<vk::DescriptorImageInfo> imageInfo;
		imageInfo.reserve(m_imageWrites.size());
		for (auto& [bindingPos, image] : m_imageWrites)
		{
			vk::DescriptorImageInfo& writeImageInfo = imageInfo.emplace_back();
			writeImageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			writeImageInfo.imageView = image->view();

			vk::WriteDescriptorSet& writeInfo = writes.emplace_back();
			writeInfo.dstSet = m_pool.getDescriptor(m_descNdx);
			writeInfo.descriptorType = vk::DescriptorType::eStorageImage;
			writeInfo.dstBinding = bindingPos;
			writeInfo.dstArrayElement = 0;
			writeInfo.descriptorCount = 1;
			writeInfo.pImageInfo = &writeImageInfo;
		}

		// Write all
		RenderContextVk().device().updateDescriptorSets(writes, {});
	}
}