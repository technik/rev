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
#include "gltfLoad.h"
#include "gltfTypes.h"
#include <core/tools/log.h>
#include <core/types/json.h>
#include <game/scene/transform/transform.h>
#include <game/scene/LightComponent.h>
#include <game/scene/meshRenderer.h>
#include <graphics/scene/renderGeom.h>
#include <graphics/scene/renderObj.h>
#include <graphics/renderer/material/Effect.h>
#include <graphics/renderer/material/material.h>

using Json = rev::core::Json;

using namespace rev::graphics;
using namespace rev::math;

namespace rev { namespace game {

	//----------------------------------------------------------------------------------------------
	void loadGLTFScene(SceneNode* _parentNode, const std::string& assetsFolder, const std::string& fileName, graphics::RenderScene& _gfxWorld)
	{
		core::File sceneFile(assetsFolder + fileName + ".gltf");
		if(!sceneFile.sizeInBytes())
		{
			core::Log::error("Unable to find scene asset");
			return;
		}
		Json sceneDesc = Json::parse(sceneFile.bufferAsText());
		auto asset = sceneDesc.find("asset");
		if(asset == sceneDesc.end())
		{
			core::Log::error("Can't find asset descriptor");
			return;
		}
		if(asset.value()["version"] != "2.0")
		{
			core::Log::error("Wrong format version. GLTF assets must be 2.0");
			return;
		}
		auto scene = sceneDesc.find("scene");
		auto scenesDict = sceneDesc.find("scenes");
		if(scenesDict == sceneDesc.end() || scene == sceneDesc.end() || scenesDict.value().size() == 0)
		{
			core::Log::error("Can't find proper scene descriptor in asset's scenes");
			return;
		}
		// Load buffers
		std::vector<gltf::Buffer> buffers;
		for(auto& buffDesc : sceneDesc["buffers"])
			buffers.emplace_back(assetsFolder, buffDesc);

		// Load buffer views
		std::vector<gltf::BufferView> bufferViews;
		for(auto& viewDesc : sceneDesc["bufferViews"])
		{
			gltf::BufferView view;
			size_t offset = viewDesc["byteOffset"];
			size_t bufferNdx = viewDesc["buffer"];
			if(viewDesc.find("byteStride") != viewDesc.end())
			{
				view.stride = viewDesc["byteStride"];
			}
			view.data = &buffers[bufferNdx].raw[offset];
			bufferViews.push_back(view);
		}

		// Load accessors
		std::vector<gltf::Accessor> accessors;
		for(auto& accessorDesc : sceneDesc["accessors"])
		{
			gltf::Accessor accessor;
			accessor.type = accessorDesc["type"].get<std::string>();
			unsigned cTypeDesc = accessorDesc["componentType"];
			accessor.componentType = gltf::Accessor::ComponentType(cTypeDesc);
			unsigned bufferViewNdx = accessorDesc["bufferView"];
			auto offsetIter = accessorDesc.find("byteOffset");
			if(offsetIter != accessorDesc.end())
				accessor.offset = offsetIter.value().get<unsigned>();
			accessor.count = accessorDesc["count"];
			accessor.view = &bufferViews[bufferViewNdx];
			accessors.push_back(accessor);
		}

		// Load textures
		std::vector<std::string>	textureNames;
		for(auto& texDesc : sceneDesc["textures"])
		{
			size_t ndx = texDesc["source"];
			auto texName = assetsFolder + sceneDesc["images"][ndx]["uri"].get<std::string>();
			textureNames.push_back(texName);
		}

		// Default material
		auto pbrEffect = Effect::loadFromFile("metal-rough.fx");
		auto defaultMaterial = std::make_shared<Material>(pbrEffect);

		// Load materials
		std::vector<std::shared_ptr<graphics::Material>> materials;
		for(auto& matDesc : sceneDesc["materials"])
		{
			auto mat = std::make_shared<Material>(pbrEffect);
			if(matDesc.find("pbrMetallicRoughness") != matDesc.end())
			{
				auto pbrDesc = matDesc["pbrMetallicRoughness"];
				if(pbrDesc.find("baseColorTexture") != pbrDesc.end())
				{
					size_t albedoNdx = pbrDesc["baseColorTexture"]["index"];
					mat->addTexture("uBaseColorMap", Texture::load(textureNames[albedoNdx], false));
				}
				if(pbrDesc.find("metallicRoughnessTexture") != pbrDesc.end())
				{
					size_t physicsNdx = pbrDesc["metallicRoughnessTexture"]["index"];
					mat->addTexture("uPhysics", Texture::load(textureNames[physicsNdx], false));
				}
				if(pbrDesc.find("baseColorFactor") != pbrDesc.end())
				{
					auto& baseColorDesc = pbrDesc["baseColorFactor"];
					Vec4f color {
						baseColorDesc[0].get<float>(),
						baseColorDesc[1].get<float>(),
						baseColorDesc[2].get<float>(),
						baseColorDesc[3].get<float>()
					};
					mat->addParam("uBaseColor", color);
				}
			}
			if(matDesc.find("emissiveTexture") != matDesc.end())
			{
				size_t emissiveNdx = matDesc["emissiveTexture"]["index"];
				mat->addTexture("uEmissive", Texture::load(textureNames[emissiveNdx], false));
			}
			if(matDesc.find("normalTexture") != matDesc.end())
			{
				size_t normalNdx = matDesc["normalTexture"]["index"];
				mat->addTexture("uNormalMap", Texture::load(textureNames[normalNdx], false));
			}
			materials.push_back(mat);
		}

		// Load meshes
		std::vector<gltf::Mesh>	meshes;
		for(auto& meshDesc : sceneDesc["meshes"])
		{
			meshes.emplace_back(accessors, meshDesc);
		}

		// Load lights
		std::vector<gltf::Light> lights;
		for(auto& lightDesc : sceneDesc["lights"])
		{
			lights.emplace_back(lightDesc);
		}

		// Load nodes
		std::vector<std::shared_ptr<SceneNode>> nodes;
		for(auto& nodeDesc : sceneDesc["nodes"])
		{
			auto node = std::make_shared<SceneNode>();
			nodes.push_back(node);
			// Node name
			auto nameIter = nodeDesc.find("name");
			if(nameIter != nodeDesc.end())
				node->name = nameIter.value().get<std::string>();
			// Node transform
			auto nodeTransform = std::make_unique<game::Transform>();
			bool useTransform = false;
			if(nodeDesc.find("matrix") != nodeDesc.end())
			{
				useTransform = true;
				auto& matrixDesc = nodeDesc["matrix"];
				for(size_t i = 0; i < 3; ++i)
					for(size_t j = 0; j < 4; ++j)
						nodeTransform->xForm.matrix()(i,j) = matrixDesc[i+4*j].get<float>();
			}
			if(nodeDesc.find("rotation") != nodeDesc.end())
			{
				useTransform = true;
				auto& rotDesc = nodeDesc["rotation"];
				Quatf rot {
					rotDesc[0].get<float>(),
					rotDesc[1].get<float>(),
					rotDesc[2].get<float>(),
					rotDesc[3].get<float>()
				};
				nodeTransform->xForm.matrix().block<3,3>(0,0) = (Mat33f)rot;
			}
			if(nodeDesc.find("translation") != nodeDesc.end())
			{
				useTransform = true;
				auto& posDesc = nodeDesc["translation"];
				for(size_t i = 0; i < 3; ++i)
					nodeTransform->xForm.matrix()(i,3) = posDesc[i].get<float>();
			}
			if(nodeDesc.find("scale") != nodeDesc.end())
			{
				useTransform = true;
				auto& scaleDesc = nodeDesc["scale"];
				Mat33f scale = Mat33f::identity();
				for(size_t i = 0; i < 3; ++i)
					scale(i,i) = scaleDesc[i].get<float>();
				nodeTransform->xForm.matrix().block<3,3>(0,0) = nodeTransform->xForm.matrix().block<3,3>(0,0) * scale;
			}
			if(useTransform)
				node->addComponent(std::move(nodeTransform));
			// Optional light
			if(nodeDesc.find("light") != nodeDesc.end())
			{
				int ndx = nodeDesc["light"].get<int>();
				auto& l = lights[ndx];
				if(l.mType == gltf::Light::Spot)
				{
					node->addComponent<game::SpotLight>(_gfxWorld, l.mAngle, l.mRange, l.mColor);
				}
			}
			// Optional node mesh
			auto meshIter = nodeDesc.find("mesh");
			if(meshIter != nodeDesc.end())
			{
				auto renderObj = std::make_shared<graphics::RenderObj>();

				size_t meshNdx = meshIter.value();
				auto& gltfMesh = meshes[meshNdx];
				for(auto& primitive : gltfMesh.primitives)
				{
					// Copy index data
					std::vector<uint16_t> indices(primitive.indices->count);
					if(primitive.indices->componentType == gltf::Accessor::ComponentType::UNSIGNED_SHORT)
					{
						memcpy(indices.data(), primitive.indices->view->data, sizeof(uint16_t)*indices.size());
					}
					else
						return;

					std::vector<RenderGeom::Vertex> vertices(primitive.position->count);
					for(size_t i = 0; i < vertices.size(); ++i)
					{
						auto& v = vertices[i];
						assert(primitive.position->componentType == gltf::Accessor::ComponentType::FLOAT);
						assert(primitive.normal->componentType == gltf::Accessor::ComponentType::FLOAT);
						assert(primitive.tangent->componentType == gltf::Accessor::ComponentType::FLOAT);
						assert(primitive.texCoord->componentType == gltf::Accessor::ComponentType::FLOAT);
						v.position = *(math::Vec3f*)(primitive.position->element(i));
						v.normal = *(math::Vec3f*)(primitive.normal->element(i));
						auto srcTangent = *(math::Vec4f*)(primitive.tangent->element(i));
						v.tangent = -Vec3f(srcTangent.block<3,1>(0,0));
						v.bitangent = v.normal.cross(v.tangent)*srcTangent.w();
						v.uv = *(math::Vec2f*)(primitive.texCoord->element(i));
					}

					// TODO: Share meshes
					renderObj->meshes.push_back(std::make_shared<RenderGeom>(vertices,indices));
					if(materials.empty())
					{
						renderObj->materials.push_back(defaultMaterial);
					}
					else
						renderObj->materials.push_back(materials[0]); // TODO
				}

				auto nodeMesh = std::make_unique<game::MeshRenderer>(renderObj);
				_gfxWorld.renderables().push_back(renderObj);

				node->addComponent(std::move(nodeMesh));
			}
		}
		// Rebuild hierarchy
		size_t i = 0;
		for(auto& nodeDesc : sceneDesc["nodes"])
		{
			auto& node = nodes[i++];
			auto childIter = nodeDesc.find("children");
			if(childIter != nodeDesc.end())
			{
				for(auto& child : childIter.value())
					node->addChild(nodes[child.get<size_t>()]);
			}
		}

		// Return the right scene
		if(_parentNode)
		{
			auto& displayScene = scenesDict.value()[(unsigned)scene.value()];
			auto& sceneNodes = displayScene["nodes"];
			auto nChildren = sceneNodes.size();
			for(size_t i = 0; i < nChildren; ++i)
				_parentNode->addChild(nodes[sceneNodes[i]]);
		}
	}
}}
