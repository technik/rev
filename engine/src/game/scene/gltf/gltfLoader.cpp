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
				material.aoTexture = gltfMaterial.occlusionTexture.index;
				material.emissiveTexture = gltfMaterial.emissiveTexture.index;
				material.normalTexture = gltfMaterial.normalTexture.index;

				result.push_back(material);
			}

			return result;
		}

		auto loadImages(const std::string assetFolder, const gltf::Document& document)
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
					images.push_back(Image::load(assetFolder + "/"+ gltfImage.uri, 4, isSRGB[imgNdx]));
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
				// Locate index data
				auto indices = loadIndices(document, primitive.indices, gltf::BufferView::TargetType::ElementArrayBuffer);

				auto p = rasterDataDst.addPrimitiveData(
					(uint32_t)vtxPos.size(),
					vtxPos.data(),
					vtxNormal.data(),
					vtxTangents.empty() ? nullptr : vtxTangents.data(),
					texCoord.data(),
					(uint32_t)indices.size(), indices.data());
				firstPrimitive = min(firstPrimitive, p);
				lastPrimitive = p;
			}

			result.meshInstances.addMesh(firstPrimitive, lastPrimitive + 1);
		}

		// Load resources
		result.materials = loadMaterials(document);
		auto images = loadImages(m_assetsFolder, document);
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