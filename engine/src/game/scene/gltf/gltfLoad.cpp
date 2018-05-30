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
#include "gltf.h"
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

using namespace fx;
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
		const gltf::Document& _document,
		shared_ptr<Effect> _pbrEffect,
		const std::vector<std::shared_ptr<Texture>>& _textures,
		std::vector<std::shared_ptr<Material>>& _materials
		)
	{
		// Load materials
		for(auto& matDesc : _document.materials)
		{
			auto mat = std::make_shared<Material>(_pbrEffect);
			auto& pbrDesc = matDesc.pbrMetallicRoughness;
			if(!pbrDesc.empty())
			{
				// Base color
				if(!pbrDesc.baseColorTexture.empty())
				{
					auto albedoNdx = pbrDesc.baseColorTexture.index;
					mat->addTexture("uBaseColorMap", _textures[albedoNdx]);
				}
				// Base color factor
				{
					auto& colorDesc = pbrDesc.baseColorFactor;
					auto& color = reinterpret_cast<const math::Vec4f&>(colorDesc);
					if(color != Vec4f::ones())
						mat->addParam("uBaseColor", color);
				}
				// Metallic-roughness
				if(!pbrDesc.metallicRoughnessTexture.empty())
				{
					// Load map in linear space!!
					auto ndx = pbrDesc.metallicRoughnessTexture.index;
					mat->addTexture("uPhysics", _textures[ndx]);
				}
				if(pbrDesc.roughnessFactor != 1.f)
					mat->addParam("uRoughness", pbrDesc.roughnessFactor);
				if(pbrDesc.metallicFactor != 1.f)
					mat->addParam("uMetallic", pbrDesc.metallicFactor);
			}
			if(!matDesc.emissiveTexture.empty())
				mat->addTexture("uEmissive", _textures[matDesc.emissiveTexture.index]);
			if(!matDesc.normalTexture.empty())
			{
				// TODO: Load normal map in linear space!!
				mat->addTexture("uNormalMap", _textures[matDesc.normalTexture.index]);
			}
			_materials.push_back(mat);
		}
	}

	//----------------------------------------------------------------------------------------------
	// TODO: This method assumes the texture is sRGB.
	// Instead, textures should be loaded on demand, when real color space info is available, or a first pass
	// should be performed on materials, marking textures with their corresponding color spaces
	void loadTextures(const gltf::Document& _document, std::vector<std::shared_ptr<Texture>>& textures)
	{
		textures.reserve(_document.textures.size());
		for(auto& textDesc : _document.textures)
		{
			// TODO: Use texture sampler information
			//auto& sampler = _document.samplers[textDesc.sampler];
			auto& image = _document.images[textDesc.source];
			textures.push_back(Texture::load(image.uri));
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
		auto fileName = assetsFolder + fileName + ".gltf";
		core::File sceneFile(fileName);
		if(!sceneFile.sizeInBytes())
		{
			core::Log::error("Unable to find scene asset");
			return;
		}
		// Load gltf document
		gltf::Document document = gltf::detail::Create(
			Json::parse(sceneFile.bufferAsText()),
			{ assetsFolder, {}});

		// Verify document is supported
		auto asset = document.asset;
		if(asset.empty())
		{
			core::Log::error("Can't find asset descriptor");
			return;
		}
		if(asset.version != "2.0")
		{
			core::Log::error("Wrong format version. GLTF assets must be 2.0");
			return;
		}

		// Preload all textures in the document.
		std::vector<std::shared_ptr<Texture>> textures;
		loadTextures(document, textures);

		// Create default material
		auto pbrEffect = Effect::loadFromFile("metal-rough.fx");
		auto defaultMaterial = std::make_shared<Material>(pbrEffect);

		// Load materials
		std::vector<std::shared_ptr<graphics::Material>> materials;
		loadMaterials(document, pbrEffect, textures, materials);

		// Load nodes
		std::vector<std::shared_ptr<SceneNode>> nodes;
		loadNodes(document.nodes, nodes, meshes, materials, defaultMaterial, lights, _gfxWorld);
		
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
