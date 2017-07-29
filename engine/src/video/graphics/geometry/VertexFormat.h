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

		enum class UnitVecFormat : size_t {
			eNone = 0,
			e3Vec3f = 3,
			// Compressed formats
			ePyramid2f = 2, // Hemioctahedron compression
			eOcta2f = 2, // Octahedron compression
			eXZ2f = 2 // Store X and Z components, Y must be reconstructed
		};

		enum class FloatStorage : size_t {
			eFloat32 = 4,
			eFloat16 = 2,
		};

		enum class NormalSpace {
			eModel,
			eTangent
		};

		bool			hasPosition = false;
		UnitVecFormat	normalFmt = UnitVecFormat::eNone;
		FloatStorage	normalStorage = FloatStorage::eFloat32;
		NormalSpace		normalSpace = NormalSpace::eModel;
		size_t			nUVs = 0;

		size_t	stride() {
			size_t stride = 0;
			if(hasPosition)
				stride += 3*sizeof(float);
			stride += (size_t)normalFmt * (size_t)normalStorage;
			stride += 2*sizeof(float)*nUVs;
			return stride;
		}

#ifdef REV_USE_VULKAN
		VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription bindingDescription = {};

			bindingDescription.binding = 0;
			bindingDescription.stride = stride();
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
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
