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
#include "gltfLoader.h"

#include "gltf.h"
#include <filesystem>

#include <core/platform/fileSystem/fileSystem.h>
#include <core/platform/fileSystem/file.h>
#include <game/scene/meshRenderer.h>
#include <game/scene/transform/transform.h>
#include <graphics/backend/Vulkan/gpuBuffer.h>
#include <graphics/backend/Vulkan/renderContextVulkan.h>
#include <graphics/backend/Vulkan/vulkanAllocator.h>
#include <graphics/RasterHeap.h>
#include <graphics/Image.h>
#include <graphics/scene/Material.h>

/*
#include <core/tasks/threadPool.h>
#include <core/tools/log.h>
#include <core/string_util.h>
#include <nlohmann/json.hpp>
#include <game/animation/skeleton.h>
#include <game/resources/load.h>
#include <game/scene/LightComponent.h>
#include <game/scene/camera.h>
#include <game/scene/transform/flyby.h>
#include <graphics/scene/animation/skinning.h>
#include <graphics/renderer/material/Effect.h>
#include <graphics/renderer/material/material.h>
#include <vector>*/

using Json = nlohmann::json;

using namespace fx;
using namespace rev::gfx;
using namespace rev::math;
using namespace std;

namespace rev::game {

	namespace
	{
		math::Vec3f loadVec3f(const Json& v)
		{
			return Vec3f{
				v[0].get<float>(),
				v[1].get<float>(),
				v[2].get<float>()
			};
		}

		bool loadNodeTransform(const gltf::Node& _nodeDesc, Mat44f& dst)
		{
			dst = Mat44f::identity();
			bool useTransform = false;
			if (!_nodeDesc.matrix.empty())
			{
				useTransform = true;
				auto& matrixDesc = _nodeDesc.matrix;
				for (size_t i = 0; i < 3; ++i)
					for (size_t j = 0; j < 4; ++j)
						dst(i, j) = matrixDesc[i + 4 * j];
			}
			if (!_nodeDesc.rotation.empty())
			{
				useTransform = true;
				Quatf rot = *reinterpret_cast<const Quatf*>(_nodeDesc.rotation.data());
				dst.block<3, 3, 0, 0>() = (Mat33f)rot;
			}
			if (!_nodeDesc.translation.empty())
			{
				useTransform = true;
				for (size_t i = 0; i < 3; ++i)
					dst(i, 3) = _nodeDesc.translation[i];
			}
			if (!_nodeDesc.scale.empty())
			{
				useTransform = true;
				Mat33f scale = Mat33f::identity();
				for (size_t i = 0; i < 3; ++i)
					scale(i, i) = _nodeDesc.scale[i];
				dst.block<3, 3, 0, 0>() = dst.block<3, 3, 0, 0>() * scale;
			}
			return useTransform;
		}

		template<class T>
		struct AccessorTraits;

		template<>
		struct AccessorTraits<Vec4f>
		{
			static constexpr gltf::Accessor::ComponentType componentType = gltf::Accessor::ComponentType::Float;
			static constexpr gltf::Accessor::Type type = gltf::Accessor::Type::Vec4;
		};

		template<>
		struct AccessorTraits<Vec3f>
		{
			static constexpr gltf::Accessor::ComponentType componentType = gltf::Accessor::ComponentType::Float;
			static constexpr gltf::Accessor::Type type = gltf::Accessor::Type::Vec3;
		};

		template<>
		struct AccessorTraits<Vec2f>
		{
			static constexpr gltf::Accessor::ComponentType componentType = gltf::Accessor::ComponentType::Float;
			static constexpr gltf::Accessor::Type type = gltf::Accessor::Type::Vec2;
		};

		template<>
		struct AccessorTraits<uint8_t>
		{
			static constexpr gltf::Accessor::ComponentType componentType = gltf::Accessor::ComponentType::UnsignedByte;
			static constexpr gltf::Accessor::Type type = gltf::Accessor::Type::Scalar;
		};

		template<>
		struct AccessorTraits<uint16_t>
		{
			static constexpr gltf::Accessor::ComponentType componentType = gltf::Accessor::ComponentType::UnsignedShort;
			static constexpr gltf::Accessor::Type type = gltf::Accessor::Type::Scalar;
		};

		template<>
		struct AccessorTraits<uint32_t>
		{
			static constexpr gltf::Accessor::ComponentType componentType = gltf::Accessor::ComponentType::UnsignedInt;
			static constexpr gltf::Accessor::Type type = gltf::Accessor::Type::Scalar;
		};

		template<class Element>
		std::vector<Element> extractBufferData(const gltf::Document& document, uint32_t accessorNdx, gltf::BufferView::TargetType expectedTarget)
		{
			std::vector<Element> data;
			const auto& accessor = document.accessors[accessorNdx];
			assert(accessor.componentType == AccessorTraits<Element>::componentType);
			assert(accessor.type == AccessorTraits<Element>::type);

			const auto& bufferView = document.bufferViews[accessor.bufferView];
			assert(expectedTarget == bufferView.target || bufferView.target == gltf::BufferView::TargetType::None);
			const auto& buffer = document.buffers[bufferView.buffer];

			if(bufferView.byteStride == 0 || bufferView.byteStride == sizeof(Element)) // Tightly packed array
			{
				data.resize(accessor.count);
				memcpy(data.data(), buffer.data.data() + bufferView.byteOffset + accessor.byteOffset, accessor.count * sizeof(Element));
			}
			else // Interleaved array
			{
				data.reserve(accessor.count);
				auto rawData = reinterpret_cast<const uint8_t*>(buffer.data.data()) + bufferView.byteOffset + accessor.byteOffset;
				for (uint32_t i = 0; i < accessor.count; ++i)
				{
					data.push_back(reinterpret_cast<const Element&>(*rawData));
					rawData += bufferView.byteStride;
				}
			}

			return data;
		}

		template<class Element>
		std::vector<Element> extractBufferData(const gltf::Document& document, const gltf::Attributes& attributes, const char* attributeTag, gltf::BufferView::TargetType expectedTarget)
		{
			// Locate the right accessor
			auto iter = attributes.find(attributeTag);
			if (iter == attributes.end())
			{
				return {};
			}

			return extractBufferData<Element>(document, iter->second, expectedTarget);
		}

		std::vector<uint32_t> loadIndices(const gltf::Document& document, uint32_t accessorNdx, gltf::BufferView::TargetType expectedTarget)
		{
			std::vector<uint32_t> result;
			const auto& accessor = document.accessors[accessorNdx];
			if (accessor.componentType == gltf::Accessor::ComponentType::UnsignedByte)
			{
				auto shortIndices = extractBufferData<uint8_t>(document, accessorNdx, expectedTarget);
				result.reserve(shortIndices.size());
				for (auto index : shortIndices) result.push_back(index);
			} else if (accessor.componentType == gltf::Accessor::ComponentType::UnsignedShort)
			{
				auto shortIndices = extractBufferData<uint16_t>(document, accessorNdx, expectedTarget);
				result.reserve(shortIndices.size());
				for (auto index : shortIndices) result.push_back(index);
			}
			else
			{
				result = extractBufferData<uint32_t>(document, accessorNdx, expectedTarget);
			}

			return result;
		}

		std::vector<gfx::PBRMaterial> loadMaterials(const gltf::Document& document)
		{
			std::vector<gfx::PBRMaterial> result;
			result.reserve(document.materials.size());

			for (auto& gltfMaterial : document.materials)
			{
				PBRMaterial material;
				material.baseColor_a = reinterpret_cast<const Vec4f&>(gltfMaterial.pbrMetallicRoughness.baseColorFactor);
				material.metalness = gltfMaterial.pbrMetallicRoughness.metallicFactor;
				material.roughness = gltfMaterial.pbrMetallicRoughness.roughnessFactor;
				material.baseColorTexture = gltfMaterial.pbrMetallicRoughness.baseColorTexture.index;
				material.pbrTexture = gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index;

				result.push_back(material);
			}

			return result;
		}

		auto loadImages(const gltf::Document& document)
		{
			// Check where images are used to decide whether to mark their format as srgb or not
			std::vector<bool> isSRGB(document.images.size(), false); // Linear by default
			for (auto& mat : document.materials)
			{
				if(mat.emissiveTexture.index >= 0)
				{
					auto& tex = document.textures[mat.emissiveTexture.index];
					if (tex.source >= 0)
						isSRGB[tex.source] = true;
				}

				if (mat.pbrMetallicRoughness.baseColorTexture.index >= 0)
				{
					auto& tex = document.textures[mat.pbrMetallicRoughness.baseColorTexture.index];
					if (tex.source >= 0)
						isSRGB[tex.source] = true;
				}
			}

			std::vector<std::shared_ptr<Image>> images;
			images.reserve(document.images.size());

			uint32_t imgNdx = 0;
			for(auto& gltfImage : document.images)
			{
				if (!gltfImage.uri.empty()) // Load image from file
				{
					images.push_back(Image::load(gltfImage.uri, 4, isSRGB[imgNdx]));
				}
				else // Load from memory
				{
					auto& bv = document.bufferViews[gltfImage.bufferView];
					auto& buffer = document.buffers[bv.buffer];
					auto image = Image::loadFromMemory(buffer.data.data() + bv.byteOffset, bv.byteLength, 4, isSRGB[imgNdx]);
					images.push_back(image);
				}
				++imgNdx;
			}

			return images;
		}

		auto loadTextures(const gltf::Document& document, const std::vector<std::shared_ptr<Image>>& images)
		{
			auto& rc = RenderContext();

			std::vector<std::shared_ptr<gfx::Texture>> textures;

			for (auto& gltfTexture : document.textures)
			{
				gltf::Sampler sampler;
				if (gltfTexture.sampler >= 0)
				{
					sampler = document.samplers[gltfTexture.sampler];
				}
				auto repeatX = sampler.wrapS == gltf::Sampler::WrappingMode::Repeat ? vk::SamplerAddressMode::eRepeat : vk::SamplerAddressMode::eClampToEdge;
				auto repeatY = sampler.wrapT == gltf::Sampler::WrappingMode::Repeat ? vk::SamplerAddressMode::eRepeat : vk::SamplerAddressMode::eClampToEdge;

				std::shared_ptr<Image> image;
				if (gltfTexture.source >= 0)
				{
					image = images[gltfTexture.source];
				}
				else
				{
					assert(false && "Empty textures unsupported");
				}

				auto texture = rc.allocator().createTexture(
					rc,
					gltfTexture.name.c_str(),
					image->size(),
					image->format(),
					repeatX,
					repeatY,
					false, // No anisotropy
					0,
					image->data(),
					vk::ImageUsageFlagBits::eSampled,
					rc.graphicsQueueFamily()
				);
				textures.push_back(texture);
			}

			return textures;
		}
	}

	//----------------------------------------------------------------------------------------------
	GltfLoader::GltfLoader(gfx::RenderContextVulkan& rc)
		: m_renderContext(rc)
		, m_alloc(rc.allocator())
	{}

	//----------------------------------------------------------------------------------------------
	GltfLoader::~GltfLoader()
	{}

	bool isBinaryGltf(const std::string& path)
	{
		return path.substr(path.size() - 4) == ".glb";
	}

	//----------------------------------------------------------------------------------------------
	auto GltfLoader::load(const std::string& filePath, RasterHeap& rasterDataDst) -> LoadResult
	{
		LoadResult result{};

		// Open file
		m_assetsFolder = filesystem::path(filePath).parent_path().string();

		// Load gltf document
		gltf::ReadQuotas limits;
		limits.MaxFileSize = 1 * 1024 * 1024 * 1024; // 1 GB
		limits.MaxBufferByteLength = limits.MaxFileSize;
		gltf::Document document = isBinaryGltf(filePath) ? gltf::LoadFromBinary(filePath, limits) : gltf::LoadFromText(filePath, limits);
		if (document.buffers.size() != 1)
		{
			std::cout << "Unable to load scene " << filePath << " with " << document.buffers.size() << " buffers." << endl
				<< "Only scenes with exactly one buffer are supported." << endl;
			return result;
		}

		// Load buffers (temporary - will be replaced by RasterHeap's internal data upload).
		const auto& gltfBuffer = document.buffers[0];
		result.m_gpuData = m_alloc.createGpuBuffer(gltfBuffer.byteLength,
			vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eStorageBuffer,
			m_renderContext.graphicsQueueFamily());

		m_alloc.reserveStreamingBuffer(gltfBuffer.byteLength);
		result.asyncLoadToken = m_alloc.asyncTransfer(*result.m_gpuData, gltfBuffer.data.data(), gltfBuffer.data.size());

		// Load node tree
		result.rootNode = loadNodes(document, result.meshInstances);

		// Load meshes
		for(const auto& mesh : document.meshes)
		{
			if (mesh.primitives.empty())
				continue;

			size_t firstPrimitive = size_t(-1);
			size_t lastPrimitive = 0;
			// Iterate over the mesh's primitives
			for (auto& primitive : mesh.primitives)
			{
				auto vtxPos = extractBufferData<Vec3f>(document, primitive.attributes, "POSITION", gltf::BufferView::TargetType::ArrayBuffer); // Locate vertex data
				auto vtxNormal = extractBufferData<Vec3f>(document, primitive.attributes, "NORMAL", gltf::BufferView::TargetType::ArrayBuffer); // Locate normal data
				auto vtxTangents= extractBufferData<Vec4f>(document, primitive.attributes, "TANGENT", gltf::BufferView::TargetType::ArrayBuffer); // Locate tangent data
				auto texCoord = extractBufferData<Vec2f>(document, primitive.attributes, "TEXCOORD_0", gltf::BufferView::TargetType::ArrayBuffer); // Locate UVs
				if (texCoord.empty())
					texCoord.resize(vtxPos.size(), {});
				if (vtxTangents.empty())
				{
					//assert(false && "Tangent generation not supported");
					vtxTangents.resize(vtxPos.size(), {});
				}
				// Locate index data
				auto indices = loadIndices(document, primitive.indices, gltf::BufferView::TargetType::ElementArrayBuffer);

				auto p = rasterDataDst.addPrimitiveData(
					(uint32_t)vtxPos.size(), vtxPos.data(), vtxNormal.data(), vtxTangents.data(), texCoord.data(),
					(uint32_t)indices.size(), indices.data());
				firstPrimitive = min(firstPrimitive, p);
				lastPrimitive = p;
			}

			result.meshInstances.addMesh(firstPrimitive, lastPrimitive + 1);
		}

		// Load resources
		result.materials = loadMaterials(document);
		auto images = loadImages(document);
		result.textures = loadTextures(document, images);
		// Load materials
		return result;
	}

	//----------------------------------------------------------------------------------------------
	shared_ptr<SceneNode> GltfLoader::loadNodes(const gltf::Document& _document, MeshRenderer& meshes)
	{
		vector<shared_ptr<SceneNode>> sceneNodes;
		for (auto& nodeDesc : _document.nodes)
		{
			auto node = std::make_shared<SceneNode>();
			sceneNodes.push_back(node);

			node->name = nodeDesc.name; // Node name
		}

		// Build hierarchy
		size_t i = 0;
		for (auto& nodeDesc : _document.nodes)
		{
			auto& node = sceneNodes[i++];
			for (auto& child : nodeDesc.children)
				node->addChild(sceneNodes[child]);
		}

		i = 0;
		// Create node and add basic components
		for (auto& nodeDesc : _document.nodes)
		{
			auto& node = sceneNodes[i++];

			// Optional node transform
			Mat44f nodeFromParent;
			if (loadNodeTransform(nodeDesc, nodeFromParent))
			{
				auto transform = node->addComponent<Transform>();
				transform->xForm.matrix() = nodeFromParent;
			}

			// Optional node mesh
			if (nodeDesc.mesh >= 0)
			{
				meshes.addInstance(node->component<Transform>(), nodeDesc.mesh);
			}
		}

		auto& scene = _document.scenes[_document.scene];
		if (scene.nodes.size() == 1)
			return sceneNodes[scene.nodes.front()];
		else
		{
			auto rootNode = make_shared<SceneNode>("Gltf root");
			for (auto id : scene.nodes)
				rootNode->addChild(sceneNodes[id]);

			return rootNode;
		}
	}

	/*

	//----------------------------------------------------------------------------------------------
	auto loadSkin(
		const gltf::Document& document,
		const gltf::Skin& skinDesc,
		const vector<gfx::RenderGeom::Attribute>& attributes)
	{
		// Load skin's inverse binding matrices
		auto numJoints = skinDesc.joints.size();
		auto skin = make_shared<Skinning>();
		skin->inverseBinding.resize(numJoints);
		auto IBMs = attributes[skinDesc.inverseBindMatrices];
		for(size_t i = 0; i < numJoints; ++i)
			skin->inverseBinding[i] = IBMs.get<math::Mat44f>(i);

		// Load skin instance
		auto skinInstance = std::make_shared<SkinInstance>();
		skinInstance->skin = skin;
		skinInstance->appliedPose.resize(numJoints, Mat44f::identity());

		return skinInstance;
	}

	//----------------------------------------------------------------------------------------------
	auto loadSkins(const vector<gfx::RenderGeom::Attribute>& attributes, const gltf::Document& document)
	{
		std::vector<std::shared_ptr<SkinInstance>> skins;
		for(auto& skin : document.skins)
		{
			skins.push_back(loadSkin(document, skin, attributes));
		}

		return skins;
	}

	//----------------------------------------------------------------------------------------------
	auto readAttributes(
		const gltf::Document& _document,
		const vector<shared_ptr<gfx::RenderGeom::BufferView>>& _bufferViews)
	{
		vector<gfx::RenderGeom::Attribute> attributes(_document.accessors.size());

		for(size_t i = 0; i < attributes.size(); ++i)
		{
			auto& attr = attributes[i];
			auto& accessor = _document.accessors[i];
			auto& bufferView = _bufferViews[accessor.bufferView];
			attr.bufferView = bufferView;
			attr.stride = bufferView->byteStride;
			attr.offset = (GLvoid*)accessor.byteOffset;
			attr.componentType = (GLenum)accessor.componentType;
			attr.normalized = accessor.normalized;
			attr.count = accessor.count;
			attr.bounds.clear();

			// Detect number of components
			if(accessor.type == gltf::Accessor::Type::Scalar)
				attr.nComponents = 1;
			else if(accessor.type == gltf::Accessor::Type::Vec2)
				attr.nComponents = 2;
			else if(accessor.type == gltf::Accessor::Type::Vec3)
				attr.nComponents = 3;
			else if(accessor.type == gltf::Accessor::Type::Vec4)
				attr.nComponents = 4;
			else if(accessor.type == gltf::Accessor::Type::Mat2)
				attr.nComponents = 4;
			else if(accessor.type == gltf::Accessor::Type::Mat3)
				attr.nComponents = 9;
			else if(accessor.type == gltf::Accessor::Type::Mat4)
				attr.nComponents = 16;
			else attr.nComponents = 0;

			// Get bbox when available
			if(accessor.min.size() == 3 && accessor.max.size() == 3)
				attr.bounds = AABB(
					(Vec3f&)(*accessor.min.data()),
					(Vec3f&)(*accessor.max.data()));
		}

		return attributes;
	}

	//----------------------------------------------------------------------------------------------
	const gfx::RenderGeom::Attribute* registerAttribute(
		gfx::Device& device,
		const gltf::Primitive& _primitive,
		const char* tag,
		const vector<gfx::RenderGeom::Attribute>& _attributes)
	{
		if(auto posIt = _primitive.attributes.find(tag); posIt != _primitive.attributes.end())
		{
			auto& attribute = _attributes[posIt->second];
			auto& bv = *attribute.bufferView;
		
			if (!bv.vbo.isValid())
			{
				bv.vbo = device.allocateBuffer(
					bv.byteLength,
					Device::BufferUpdateFrequency::Static,
					Device::BufferUsageTarget::Vertex,
					bv.data);
			}

			return &attribute;
		}

		return nullptr;
	}

	//----------------------------------------------------------------------------------------------
	gfx::RenderGeom::Attribute* generateTangentSpace(
		gfx::Device& device,
		const gfx::RenderGeom::Attribute* positions,
		const gfx::RenderGeom::Attribute* uvs,
		const gfx::RenderGeom::Attribute* normals,
		const gfx::RenderGeom::Attribute* indices)
	{
		if(uvs->nComponents != 2)
		{
			core::Log::error("Only UVs with 2 components are supported");
			return nullptr;
		}
		// Accessor data
		auto tangents = new gfx::RenderGeom::Attribute;
		tangents->componentType = (GLenum)gltf::Accessor::ComponentType::Float;
		tangents->count = uvs->count;
		tangents->nComponents = 4;
		tangents->normalized = false;
		tangents->offset = 0;
		tangents->stride = 0;
		// Buffer view
		tangents->bufferView = make_shared<gfx::RenderGeom::BufferView>();
		auto& tanBv = *tangents->bufferView;
		tanBv.byteLength = tangents->count * tangents->nComponents * sizeof(float);
		tanBv.byteStride = 0;
		auto tangentVectors = new Vec4f[tangents->count];
		memset(tangentVectors, 0, tangents->count * sizeof(Vec4f)); // Clear the tangents
		tanBv.data = tangentVectors;

		// Actual computation
		auto nIndices = indices->count;
		auto indexData = new uint32_t[indices->count];
		if(indices->componentType == (GLenum)gltf::Accessor::ComponentType::UnsignedByte)
		{
			for(int i = 0; i < nIndices; ++i)
				indexData[i] = uint32_t(indices->get<uint8_t>(i));
		}
		else if(indices->componentType == (GLenum)gltf::Accessor::ComponentType::UnsignedShort)
		{
			for(int i = 0; i < nIndices; ++i)
				indexData[i] = uint32_t(indices->get<uint16_t>(i));
		}
		else
		{
			for(int i = 0; i < nIndices; ++i)
				indexData[i] = uint32_t(indices->get<uint32_t>(i));
		}
		// Accumulate per-triangle normals
		for(int i = 0; i < nIndices;i += 3) // Iterate over all triangles
		{
			auto i0 = indexData[i+0];
			auto i1 = indexData[i+1];
			auto i2 = indexData[i+2];

			Vec2f localUvs[3] = { uvs->get<Vec2f>(i0), uvs->get<Vec2f>(i1), uvs->get<Vec2f>(i2) };
			Vec3f localPos[3] = { positions->get<Vec3f>(i0), positions->get<Vec3f>(i1), positions->get<Vec3f>(i2) };

			Vec2f deltaUV1 = localUvs[1] - localUvs[0];
			Vec2f deltaUV2 = localUvs[2] - localUvs[0];

			Vec3f deltaPos1 = localPos[1] - localPos[0];
			Vec3f deltaPos2 = localPos[2] - localPos[0];

			auto determinant = deltaUV1.x()*deltaUV2.y()-deltaUV2.x()*deltaUV1.y();

			// Unnormalized tangent
			Vec3f triangleTangent = (deltaPos1 * deltaUV1.x() - deltaUV1.y() * deltaPos2) * (1 / determinant);

			tangentVectors[i0] = tangentVectors[i0] + Vec4f(triangleTangent.x(), triangleTangent.y(), triangleTangent.z(), determinant);
			tangentVectors[i1] = tangentVectors[i1] + Vec4f(triangleTangent.x(), triangleTangent.y(), triangleTangent.z(), determinant);
			tangentVectors[i2] = tangentVectors[i2] + Vec4f(triangleTangent.x(), triangleTangent.y(), triangleTangent.z(), determinant);
		}

		// Orthonormalize per vertex
		for(int i = 0; i < tangents->count; ++i)
		{
			auto& tangent = tangentVectors[i];
			Vec3f tangent3 = { tangent.x(), tangent.y(), tangent.z() };
			auto& normal = normals->get<Vec3f>(i);
			
			tangent3 = tangent3 - (dot(tangent3, normal) * normal); // Orthogonal tangent
			tangent3 = normalize(tangent3); // Orthonormal tangent
			tangent = { tangent3.x(), tangent3.y(), tangent3.z(), signbit(-tangent.w()) ? -1.f : 1.f };
		}

		// TODO: Allocate buffer in graphics driver
		tanBv.vbo = device.allocateBuffer(
			tanBv.byteLength,
			Device::BufferUpdateFrequency::Static,
			Device::BufferUsageTarget::Vertex,
			tanBv.data);
		delete[] reinterpret_cast<const Vec4f*>(tanBv.data);
		delete[] indexData;
		tanBv.data = nullptr;

		return tangents;
	}

	//----------------------------------------------------------------------------------------------
	shared_ptr<RenderGeom> loadPrimitive(
		gfx::Device& device,
		const gltf::Document& _document,
		const vector<gfx::RenderGeom::Attribute>& _attributes,
		const gltf::Primitive& _primitive,
		bool _needsTangentSpace)
	{
		// Early out for invalid data
		if(_primitive.indices < 0)
			return nullptr;

		const gfx::RenderGeom::Attribute* indices = &_attributes[_primitive.indices];
		if(!indices->bufferView->vbo.isValid())
		{
			auto& bv = *indices->bufferView;
			indices->bufferView->vbo = device.allocateBuffer(
				bv.byteLength,
				Device::BufferUpdateFrequency::Static,
				Device::BufferUsageTarget::Index,
				bv.data);
		}

		// Read primitive attributes
		const gfx::RenderGeom::Attribute* position = registerAttribute(device, _primitive, "POSITION", _attributes);
		const gfx::RenderGeom::Attribute* normals = registerAttribute(device, _primitive, "NORMAL", _attributes);
		const gfx::RenderGeom::Attribute* tangents = registerAttribute(device, _primitive, "TANGENT", _attributes);
		const gfx::RenderGeom::Attribute* uv0 = registerAttribute(device, _primitive, "TEXCOORD_0", _attributes);
		const gfx::RenderGeom::Attribute* weights = registerAttribute(device, _primitive, "WEIGHTS_0", _attributes);
		const gfx::RenderGeom::Attribute* joints = registerAttribute(device, _primitive, "JOINTS_0", _attributes);

		if(_needsTangentSpace && !tangents)
		{
			if(!normals || !uv0)
			{
				core::Log::error("Mesh requires tangent space but doesn't provide normals or uvs. Normal generation is not supported. Skipping primitive");
				return nullptr;
			}
			tangents = generateTangentSpace(device, position, uv0, normals, indices);
		}

		return std::make_shared<RenderGeom>(indices, position, normals, tangents, uv0, weights, joints);
	}

	//----------------------------------------------------------------------------------------------
	auto loadBufferViews(const gltf::Document& _document, const vector<core::File*>& buffers)
	{
		vector<std::shared_ptr<gfx::RenderGeom::BufferView>> bvs;
		bvs.reserve(_document.bufferViews.size());

		for(auto& bv : _document.bufferViews)
		{
			auto bufferData = reinterpret_cast<const uint8_t*>(buffers[bv.buffer]->buffer());

			gfx::RenderGeom::BufferView bufferView;
			bufferView.byteStride = (GLint)bv.byteStride;
			bufferView.data = &bufferData[bv.byteOffset];
			bufferView.byteLength = bv.byteLength;

			bvs.push_back( make_shared<gfx::RenderGeom::BufferView>(bufferView) );
		}

		return bvs;
	}

	//----------------------------------------------------------------------------------------------
	vector<shared_ptr<RenderMesh>> GltfLoader::loadMeshes(
		const vector<gfx::RenderGeom::Attribute>& attributes,
		const gltf::Document& _document,
		const vector<shared_ptr<Material>>& _materials)
	{

		// Load the meshes
		vector<shared_ptr<RenderMesh>> meshes;
		meshes.reserve(_document.meshes.size());
		for(auto& meshDesc : _document.meshes)
		{
			meshes.push_back(make_shared<RenderMesh>());
			auto mesh = meshes.back();
			for(auto& primitive : meshDesc.primitives)
			{
				auto material = defaultMaterial();
				bool needsTangentSpace = false;
				if(primitive.material >= 0)
				{
					material = _materials[primitive.material];
					needsTangentSpace = !_document.materials[primitive.material].normalTexture.empty();
				}
				auto geometry = loadPrimitive(m_gfxDevice, _document, attributes, primitive, needsTangentSpace);
				mesh->mPrimitives.emplace_back(geometry, material);
			}
			mesh->updateBBox();
		}

		return meshes;
	}

	//----------------------------------------------------------------------------------------------
	gfx::Texture2d GltfLoader::getTexture(
		const gltf::Document& document,
		gfx::TextureSampler defaultSampler,
		int32_t index,
		bool sRGB,
		int nChannels)
	{
		auto& texture = m_textures[index];
		if(texture.isValid()) // Already allocated, reuse
			return texture;

		// Not previously allocated, do it now
		auto textGltfDesc = document.textures[index];
		// TODO: Support custom samplers
		auto& image = m_loadedImages[textGltfDesc.source];
		texture = game::create2dTextureFromImage(image, m_gfxDevice, defaultSampler, sRGB, nChannels);
		return texture;
	}

	//----------------------------------------------------------------------------------------------
	std::vector<std::shared_ptr<gfx::Material>> GltfLoader::loadMaterials(
		const gltf::Document& _document
		)
	{
		std::vector<std::shared_ptr<Material>> materials;

		// Create default texture sampler
		gfx::TextureSampler::Descriptor samplerDesc;
		samplerDesc.wrapS = gfx::TextureSampler::Wrap::Repeat;
		samplerDesc.wrapT = gfx::TextureSampler::Wrap::Repeat;
		auto defSampler = m_gfxDevice.createTextureSampler(samplerDesc);

		// Create a material descriptor we can reuse for all materials
		Material::Descriptor matDesc;
		
		// Load materials
		for(auto& matData : _document.materials)
		{
			matDesc.reset();
			if(matData.alphaMode == gltf::Material::AlphaMode::Blend)
			{
				matDesc.transparency = Material::Transparency::Blend;
			}
			else if (matData.alphaMode == gltf::Material::AlphaMode::Mask)
			{
				matDesc.transparency = Material::Transparency::Mask;
			}

			bool specular = false;
			if(matData.extensionsAndExtras.find("extensions") != matData.extensionsAndExtras.end())
			{
				auto& extensions = matData.extensionsAndExtras["extensions"];
				if(extensions.find("KHR_materials_pbrSpecularGlossiness") != extensions.end())
					specular = true;
			}
			if (specular)
				matDesc.effect = specularEffect();
			else
				matDesc.effect = metallicRoughnessEffect();

			auto& pbrDesc = matData.pbrMetallicRoughness;
			if(!pbrDesc.empty())
			{
				// Base color
				if(!pbrDesc.baseColorTexture.empty())
				{
					auto albedoNdx = pbrDesc.baseColorTexture.index;
					auto texture = getTexture(_document, defSampler, albedoNdx, true);
					if(texture.isValid())
						matDesc.textures.emplace_back("uBaseColorMap", texture, Material::Flags::Shading);
				}
				// Base color factor
				{
					auto& colorDesc = pbrDesc.baseColorFactor;
					auto& color = reinterpret_cast<const math::Vec4f&>(colorDesc);
					if(color != Vec4f::ones())
						matDesc.vec4Params.emplace_back("uBaseColor", color, Material::Flags::Shading);
				}
				// Metallic-roughness
				if(!pbrDesc.metallicRoughnessTexture.empty())
				{
					// Load map in linear space!!
					auto ndx = pbrDesc.metallicRoughnessTexture.index;
					auto texture = getTexture(_document, defSampler, ndx, false, 3);
					if(texture.isValid())
						matDesc.textures.emplace_back("uPhysics", texture, Material::Flags::Shading);
				}
				if(pbrDesc.roughnessFactor != 1.f)
					matDesc.floatParams.emplace_back("uRoughness", pbrDesc.roughnessFactor, Material::Flags::Shading);
				if(pbrDesc.metallicFactor != 1.f)
					matDesc.floatParams.emplace_back("uMetallic", pbrDesc.metallicFactor, Material::Flags::Shading);
			}
			if (!matData.emissiveTexture.empty())
			{
				auto texture = getTexture(_document, defSampler, matData.emissiveTexture.index, false, 3);
				if(texture.isValid())
					matDesc.textures.emplace_back("uEmissiveMap", texture, Material::Flags::Emissive);
			}
			if(!matData.normalTexture.empty())
			{
				auto texture = getTexture(_document, defSampler, matData.normalTexture.index, false, 3);
				if(texture.isValid())
					matDesc.textures.emplace_back("uNormalMap", texture, Material::Flags::Normals);
			}

			//mat->addTexture("uEnvBRDF", envBRDF);
			auto mat = std::make_shared<Material>(matDesc);
			materials.push_back(mat);
		}

		return materials;
	}

	//----------------------------------------------------------------------------------------------
	void loadAnimations(
		const gltf::Document& document,
		const vector<gfx::RenderGeom::Attribute>& accessors,
		std::vector<std::shared_ptr<SceneNode>>& sceneNodes,
		std::vector<std::shared_ptr<SceneNode>>& animNodes,
		vector<shared_ptr<Animation>>& _animations)
	{
		for(auto& animDesc : document.animations)
		{
			auto& animation = _animations.emplace_back(make_shared<Animation>());
			std::vector<int> usedNodes;
			// Precompute channels (TODO: Use this to generate a real skeleton)

			for(auto& channelDesc : animDesc.channels)
			{
				// Locate channel ndx
				auto targetNode = channelDesc.target.node;
				auto ndxIter = std::find(usedNodes.begin(), usedNodes.end(), targetNode);
				auto targetNdx = usedNodes.size();
				if(ndxIter != usedNodes.end())
					targetNdx = ndxIter - usedNodes.begin();
				else
					usedNodes.push_back(targetNode);
			}
			// Mark first animated node
			animNodes.push_back(sceneNodes[usedNodes[0]]);
			// Resize animation to fit all the channels
			animation->m_translationChannels.resize(usedNodes.size());
			animation->m_rotationChannels.resize(usedNodes.size());
			// Load channel contents
			for(auto& channelDesc : animDesc.channels)
			{
				auto channelNdx = std::find(usedNodes.begin(), usedNodes.end(), channelDesc.target.node) - usedNodes.begin();
				if(channelDesc.target.path == "rotation")
				{
					auto& channel = animation->m_rotationChannels[channelNdx];
					auto sampler = animDesc.samplers[channelDesc.sampler];
					auto time = accessors[sampler.input];
					auto values = accessors[sampler.output];
					for(int i = 0; i < time.count; ++i)
					{
						channel.t.push_back(time.get<float>(i));
						channel.values.push_back(values.get<math::Quatf>(i));
					}
				}
			}
		}
	}

	std::shared_ptr<Effect> GltfLoader::metallicRoughnessEffect()
	{
		if (!m_metallicRoughnessEffect)
		{
			m_metallicRoughnessEffect = std::make_shared<Effect>("shaders/metal-rough.fx");
		}
		
		return m_metallicRoughnessEffect;
	}

	std::shared_ptr<Effect> GltfLoader::specularEffect()
	{
		if (!m_specularEffect)
		{
			m_specularEffect = std::make_shared<Effect>("shaders/specularSetup.fx");
		}

		return m_specularEffect;
	}

	std::shared_ptr<Material> GltfLoader::defaultMaterial()
	{
		return nullptr;
	}

	void GltfLoader::loadImages(const gltf::Document& document)
	{
		// Allocate pointers to the images
		m_loadedImages.resize(document.images.size());

		// Load images in parallel
		core::ThreadPool workers(8);
		workers.run(document.images,
			[this](const gltf::Image& imageDesc, size_t taskId) {
				// Load image from file
				m_loadedImages[taskId] = gfx::Image::load(m_assetsFolder + imageDesc.uri, 0);
			},
			std::cout);

		// Report not found images
		for (size_t i = 0; i < document.images.size(); ++i)
		{
			if (!m_loadedImages[i])
			{
				std::cout << "Unable to load " << document.images[i].uri << "\n";
			}
		}
	}*/
}