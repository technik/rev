//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#ifdef REV_USE_VULKAN
#include <array>
#include <vulkan/vulkan.h>
#endif // REV_USE_VULKAN

#include <math/algebra/vector.h>

namespace rev { namespace video {

	struct VertexFormat {
#ifdef REV_USE_VULKAN
		static VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription bindingDescription = {};

			bindingDescription.binding = 0;
			bindingDescription.stride = 2*sizeof(math::Vec3f);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = 0;// offsetof(Vertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = sizeof(math::Vec3f);

			return attributeDescriptions;
		}
#endif // REV_USE_VULKAN
	};

}}
