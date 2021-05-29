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
		void addStorageBuffer(const std::string& name, uint32_t bindingPos, vk::ShaderStageFlags shaderStages);

		void addTexture(const std::string& name, uint32_t bindingPos, vk::ShaderStageFlags shaderStages);

		void addImage(const std::string& name, uint32_t bindingPos, vk::ShaderStageFlags shaderStages);

		void addTextureArray(const std::string& name, uint32_t bindingPos, uint32_t numTextures, vk::ShaderStageFlags shaderStages);

		void close(uint32_t poolSize);

		inline vk::DescriptorSet getDescriptor(uint32_t index) const
		{
			return m_descriptorSets[index];
		}

		inline auto layout() const { return m_vkLayout; }

		// Immediate write
		void writeArrayTextureToDescriptor(uint32_t descNdx, const std::string& name, const std::vector<std::shared_ptr<Texture>>& textureArray);

	private:

		void createDescriptorPool(uint32_t numDescriptorSets);

		void createDescriptors(uint32_t numDescriptors);

		std::vector<vk::DescriptorSetLayoutBinding> m_bindings;
		std::map<std::string, uint32_t> m_storageBufferBindings;
		std::map<std::string, uint32_t> m_textureBindings;
		std::map<std::string, uint32_t> m_imageBindings;
		std::map<std::string, std::pair<uint32_t,uint32_t>> m_textureArrayBindings;
		uint32_t m_numTextures = 0;
		uint32_t m_numImages = 0;
		uint32_t m_numStorageBuffers = 0;
		vk::DescriptorSetLayout m_vkLayout;
		vk::DescriptorPool m_pool;
		std::vector<vk::DescriptorSet> m_descriptorSets;

		friend class DescriptorSetUpdate;
	};

	// Combine multiple small writes into a single update
	class DescriptorSetUpdate
	{
	public:
		DescriptorSetUpdate(DescriptorSetLayout& layout, uint32_t descNdx) : m_layout(layout), m_descNdx(descNdx) {}

		void addStorageBuffer(const std::string& name, std::shared_ptr<GPUBuffer> buffer);

		void addTexture(const std::string& name, std::shared_ptr<Texture> texture);
		void addImage(const std::string& name, std::shared_ptr<ImageBuffer> image);

		void send() const;

	private:
		DescriptorSetLayout& m_layout;
		uint32_t m_descNdx;
		std::map<uint32_t, std::shared_ptr<GPUBuffer>> m_bufferWrites;
		std::map<uint32_t, std::shared_ptr<Texture>> m_textureWrites;
		std::map<uint32_t, std::shared_ptr<ImageBuffer>> m_imageWrites;
	};
}