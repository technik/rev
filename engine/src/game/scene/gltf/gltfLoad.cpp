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
#include <memory>
#include <vector>

using Json = rev::core::Json;

using namespace rev::graphics;
using namespace rev::math;
using namespace std;

namespace rev { namespace game {

	math::Vec3f loadVec3f(const Json& v)
	{
		return Vec3f {
			v[0].get<float>(),
			v[1].get<float>(),
			v[2].get<float>()
		};
	}

	std::unique_ptr<Transform> loadNodeTransform(const Json& _nodeDesc)
	{
		auto nodeTransform = std::make_unique<game::Transform>();
		bool useTransform = false;
		if(_nodeDesc.find("matrix") != _nodeDesc.end())
		{
			useTransform = true;
			auto& matrixDesc = _nodeDesc["matrix"];
			for(size_t i = 0; i < 3; ++i)
				for(size_t j = 0; j < 4; ++j)
					nodeTransform->xForm.matrix()(i,j) = matrixDesc[i+4*j].get<float>();
		}
		if(_nodeDesc.find("rotation") != _nodeDesc.end())
		{
			useTransform = true;
			auto& rotDesc = _nodeDesc["rotation"];
			Quatf rot {
				rotDesc[0].get<float>(),
				rotDesc[1].get<float>(),
				rotDesc[2].get<float>(),
				rotDesc[3].get<float>()
			};
			nodeTransform->xForm.matrix().block<3,3>(0,0) = (Mat33f)rot;
		}
		if(_nodeDesc.find("translation") != _nodeDesc.end())
		{
			useTransform = true;
			auto& posDesc = _nodeDesc["translation"];
			for(size_t i = 0; i < 3; ++i)
				nodeTransform->xForm.matrix()(i,3) = posDesc[i].get<float>();
		}
		if(_nodeDesc.find("scale") != _nodeDesc.end())
		{
			useTransform = true;
			auto& scaleDesc = _nodeDesc["scale"];
			Mat33f scale = Mat33f::identity();
			for(size_t i = 0; i < 3; ++i)
				scale(i,i) = scaleDesc[i].get<float>();
			nodeTransform->xForm.matrix().block<3,3>(0,0) = nodeTransform->xForm.matrix().block<3,3>(0,0) * scale;
		}
		if(useTransform)
			return std::move(nodeTransform);
		else
			return nullptr;
	}

	//-----------------------------------------------------------------------------------------------
	shared_ptr<RenderGeom> loadMesh(const gltf::Primitive& primitive)
	{
		std::vector<uint16_t> indices(primitive.indices->count);
		if(primitive.indices->componentType == gltf::Accessor::ComponentType::UNSIGNED_SHORT)
		{
			memcpy(indices.data(), primitive.indices->view->data, sizeof(uint16_t)*indices.size());
		}
		else
			return nullptr;

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

		auto mesh = std::make_shared<RenderGeom>(vertices,indices);
		if(primitive.position->hasBounds)
			mesh->bbox = BBox(primitive.position->min, primitive.position->max);
		return mesh;
	}

	using GeometryCache = std::unordered_map<const gltf::Accessor*, shared_ptr<RenderGeom>>;

	//-----------------------------------------------------------------------------------------------
	std::shared_ptr<RenderObj> loadRenderObj(
		size_t meshNdx,
		GeometryCache& cache,
		const std::vector<gltf::Mesh>&	_meshes,
		const std::vector<std::shared_ptr<Material>>& _materials,
		const std::shared_ptr<Material> _defaultMaterial)
	{
		auto renderObj = std::make_shared<RenderObj>();

		auto& gltfMesh = _meshes[meshNdx];
		for(auto& primitive : gltfMesh.primitives)
		{
			// Cache mesh creation
			shared_ptr<RenderGeom> mesh;
			auto iter = cache.find(primitive.indices);
			if(iter == cache.end())
			{
				mesh = loadMesh(primitive);
				cache.emplace(primitive.indices, mesh);
			}
			else
				mesh = iter->second;

			if(!mesh)
				return nullptr;

			// Add mesh
			renderObj->meshes.push_back(mesh);
			if(_materials.empty() || primitive.material == -1)
			{
				renderObj->materials.push_back(_defaultMaterial);
			}
			else
				renderObj->materials.push_back(_materials[ primitive.material]); // TODO: Find proper material
		}

		return renderObj;
	}

	//----------------------------------------------------------------------------------------------
	void loadNodes(
		const Json& _nodeArrayDesc,
		vector<shared_ptr<SceneNode>>& _sceneNodes,
		const std::vector<gltf::Mesh>&	_meshes,
		const std::vector<std::shared_ptr<Material>>& _materials,
		std::shared_ptr<Material> _defaultMaterial,
		const std::vector<gltf::Light>& _lights,
		graphics::RenderScene& _gfxWorld)
	{
		// Map first accessor's id, to a generated render object
		GeometryCache renderObjCache;
		for(auto& nodeDesc : _nodeArrayDesc)
		{
			auto node = std::make_shared<SceneNode>();
			_sceneNodes.push_back(node);
			// Node name
			auto nameIter = nodeDesc.find("name");
			if(nameIter != nodeDesc.end())
				node->name = nameIter.value().get<std::string>();
			// Node transform
			auto nodeTransform = loadNodeTransform(nodeDesc);
			if(nodeTransform)
				node->addComponent(std::move(nodeTransform));
			
			// Optional light
			if(nodeDesc.find("light") != nodeDesc.end())
			{
				int ndx = nodeDesc["light"].get<int>();
				auto& l = _lights[ndx];
				if(l.mType == gltf::Light::Spot)
				{
					node->addComponent<game::SpotLight>(_gfxWorld, l.mAngle, l.mRange, l.mColor);
				}
			}
			// Optional node mesh
			auto meshIter = nodeDesc.find("mesh");
			if(meshIter != nodeDesc.end())
			{
				// TODO: Try to reuse renderObj, use first primitive as key to index the cache
				size_t meshNdx = meshIter.value();
				std::shared_ptr<RenderObj>	renderObj = loadRenderObj(
					meshNdx, renderObjCache,
					_meshes, _materials, _defaultMaterial);

				auto nodeMesh = std::make_unique<game::MeshRenderer>(renderObj);
				_gfxWorld.renderables().push_back(renderObj);

				node->addComponent(std::move(nodeMesh));
			}
		}
	}

	//----------------------------------------------------------------------------------------------
	void loadMaterials(
		const Json& matArray,
		shared_ptr<Effect> _pbrEffect,
		std::vector<std::shared_ptr<Material>>& _materials,
		const std::vector<std::string>&	_textureNames
		)
	{
		// Load materials
		for(auto& matDesc : matArray)
		{
			auto mat = std::make_shared<Material>(_pbrEffect);
			if(matDesc.find("pbrMetallicRoughness") != matDesc.end())
			{
				auto pbrDesc = matDesc["pbrMetallicRoughness"];
				// Base color
				if(pbrDesc.find("baseColorTexture") != pbrDesc.end())
				{
					size_t albedoNdx = pbrDesc["baseColorTexture"]["index"];
					mat->addTexture("uBaseColorMap", Texture::load(_textureNames[albedoNdx], false));
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
				// Metallic-roughness
				if(pbrDesc.find("metallicRoughnessTexture") != pbrDesc.end())
				{
					size_t physicsNdx = pbrDesc["metallicRoughnessTexture"]["index"];
					mat->addTexture("uPhysics", Texture::load(_textureNames[physicsNdx], false));
				}
				if(pbrDesc.find("roughnessFactor") != pbrDesc.end())
					mat->addParam("uRoughness", pbrDesc["roughnessFactor"].get<float>());
				if(pbrDesc.find("metallicFactor") != pbrDesc.end())
					mat->addParam("uMetallic", pbrDesc["metallicFactor"].get<float>());
			}
			if(matDesc.find("emissiveTexture") != matDesc.end())
			{
				size_t emissiveNdx = matDesc["emissiveTexture"]["index"];
				mat->addTexture("uEmissive", Texture::load(_textureNames[emissiveNdx], false));
			}
			if(matDesc.find("normalTexture") != matDesc.end())
			{
				size_t normalNdx = matDesc["normalTexture"]["index"];
				mat->addTexture("uNormalMap", Texture::load(_textureNames[normalNdx], false));
			}
			_materials.push_back(mat);
		}
	}

	//----------------------------------------------------------------------------------------------
	void loadGLTFScene(
		SceneNode* _parentNode,
		const std::string& assetsFolder,
		const std::string& fileName,
		graphics::RenderScene& _gfxWorld,
		graphics::GeometryPool& _geomPool)
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
			bufferViews.emplace_back(buffers, viewDesc);
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
			auto minIter = accessorDesc.find("min");
			auto maxIter = accessorDesc.find("max");
			if(minIter != accessorDesc.end() && maxIter != accessorDesc.end())
			{
				accessor.min = loadVec3f(accessorDesc["min"]);
				accessor.max = loadVec3f(accessorDesc["max"]);
				accessor.hasBounds = true;
			}
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
		loadMaterials(sceneDesc["materials"], pbrEffect, materials, textureNames);

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
		loadNodes(sceneDesc["nodes"], nodes, meshes, materials, defaultMaterial, lights, _gfxWorld);
		
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
