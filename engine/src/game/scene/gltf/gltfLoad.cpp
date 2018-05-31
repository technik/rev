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
#include <graphics/scene/renderMesh.h>
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

	std::unique_ptr<Transform> loadNodeTransform(const gltf::Node& _nodeDesc)
	{
		auto nodeTransform = std::make_unique<game::Transform>();
		bool useTransform = false;
		if(!_nodeDesc.matrix.empty())
		{
			useTransform = true;
			auto& matrixDesc = _nodeDesc.matrix;
			for(size_t i = 0; i < 3; ++i)
				for(size_t j = 0; j < 4; ++j)
					nodeTransform->xForm.matrix()(i,j) = matrixDesc[i+4*j];
		}
		if(!_nodeDesc.rotation.empty())
		{
			useTransform = true;
			Quatf rot = *reinterpret_cast<const Quatf*>(_nodeDesc.rotation.data());
			nodeTransform->xForm.matrix().block<3,3>(0,0) = (Mat33f)rot;
		}
		if(!_nodeDesc.translation.empty())
		{
			useTransform = true;
			for(size_t i = 0; i < 3; ++i)
				nodeTransform->xForm.matrix()(i,3) = _nodeDesc.translation[i];
		}
		if(!_nodeDesc.scale.empty())
		{
			useTransform = true;
			Mat33f scale = Mat33f::identity();
			for(size_t i = 0; i < 3; ++i)
				scale(i,i) = _nodeDesc.scale[i];
			nodeTransform->xForm.matrix().block<3,3>(0,0) = nodeTransform->xForm.matrix().block<3,3>(0,0) * scale;
		}
		if(useTransform)
			return std::move(nodeTransform);
		else
			return nullptr;
	}

	//----------------------------------------------------------------------------------------------
	auto loadNodes(
		const gltf::Document& _document,
		const vector<shared_ptr<RenderMesh>>& _meshes,
		const vector<shared_ptr<Material>>& _materials,
		shared_ptr<Material> _defaultMaterial,
		graphics::RenderScene& _gfxWorld)
	{
		vector<shared_ptr<SceneNode>> nodes;

		// Create node and add basic components
		for(auto& nodeDesc : _document.nodes)
		{
			auto node = std::make_shared<SceneNode>();
			node->name = nodeDesc.name; // Node name

			// Optional node transform
			auto nodeTransform = loadNodeTransform(nodeDesc);
			if(nodeTransform)
				node->addComponent(std::move(nodeTransform));
			
			// Optional node mesh
			if(nodeDesc.mesh >= 0)
			{
				auto renderObj = make_shared<RenderObj>(_meshes[nodeDesc.mesh]);
				node->addComponent<MeshRenderer>(renderObj);
				_gfxWorld.renderables().push_back(renderObj);
			}

			nodes.push_back(node);
		}

		// Rebuild hierarchy
		size_t i = 0;
		for(auto& nodeDesc : _document.nodes)
		{
			auto& node = nodes[i++];
			for(auto& child : nodeDesc.children)
				node->addChild(nodes[child]);
		}

		return nodes;
	}

	//----------------------------------------------------------------------------------------------
	template<class Attr>
	vector<Attr> readAttribute(
		const gltf::Document& _document,
		const vector<core::File*> _buffers,
		uint32_t accessorNdx)
	{
		auto& accessor = _document.accessors[accessorNdx];
		auto& bufferView = _document.bufferViews[accessor.bufferView];
		auto offset = bufferView.byteOffset + accessor.byteOffset;
		auto bufferData = _buffers[bufferView.buffer]->buffer();
		auto data = reinterpret_cast<const uint8_t*>(bufferData) + offset;
		auto stride = std::max(sizeof(Attr), bufferView.byteStride);

		vector<Attr> attribute;
		if(data && bufferView.byteLength >= accessor.count * stride)
		{
			attribute.resize(accessor.count);
			for(size_t i = 0; i < accessor.count; ++i)
				attribute[i] = reinterpret_cast<const Attr&>(data[stride*i]);
		}
		return attribute;
	}

	//----------------------------------------------------------------------------------------------
	shared_ptr<RenderGeom> loadPrimitive(
		const gltf::Document& _document,
		const vector<core::File*> _buffers,
		const gltf::Primitive& _primitive)
	{
		// Read indices
		auto indices = readAttribute<uint16_t>(_document, _buffers, _primitive.indices);
		if(indices.empty()) return nullptr;

		// Read vertex data
		// TODO: Efficient read of both interleaved and separated vertex data
		// Also: use a single vertex buffer when possible
		auto posAcsNdx = _primitive.attributes.at("POSITION");
		auto position = readAttribute<Vec3f>(_document, _buffers, posAcsNdx);
		auto normal = readAttribute<Vec3f>(_document, _buffers, _primitive.attributes.at("NORMAL"));
		auto tangent = readAttribute<Vec4f>(_document, _buffers, _primitive.attributes.at("TANGENT"));
		auto uv = readAttribute<Vec2f>(_document, _buffers, _primitive.attributes.at("TEXCOORD_0"));

		// Copy vertex data into the right format
		std::vector<RenderGeom::Vertex> vertices(position.size());
		for(size_t i = 0; i < vertices.size(); ++i)
		{
			auto& v = vertices[i];
			v.position = position[i];
			v.normal = normal[i];
			auto srcTangent = tangent[i];
			v.tangent = -Vec3f(srcTangent.block<3,1>(0,0));
			v.bitangent = v.normal.cross(v.tangent)*srcTangent.w();
			v.uv = uv[i];
		}

		auto mesh = std::make_shared<RenderGeom>(vertices,indices);
			mesh->bbox = BBox(
				reinterpret_cast<const Vec4f&>(*_document.accessors[posAcsNdx].min.data()),
				reinterpret_cast<const Vec4f&>(*_document.accessors[posAcsNdx].max.data()));
		return mesh;
	}

	//----------------------------------------------------------------------------------------------
	auto loadMeshes(
		const std::string& _assetsFolder,
		const gltf::Document& _document,
		const vector<shared_ptr<Material>>& _materials)
	{
		// Load buffers
		vector<core::File*> buffers;
		for(auto b : _document.buffers)
			buffers.push_back(new core::File(_assetsFolder+b.uri));

		// Load the meshes
		vector<shared_ptr<RenderMesh>> meshes;
		meshes.reserve(_document.meshes.size());
		for(auto& meshDesc : _document.meshes)
		{
			meshes.push_back(make_shared<RenderMesh>());
			auto mesh = meshes.back();
			for(auto& primitive : meshDesc.primitives)
			{
				shared_ptr<Material> material = _materials[primitive.material];
				shared_ptr<RenderGeom> geometry = loadPrimitive(_document, buffers, primitive);
				mesh->mPrimitives.emplace_back(geometry, material);
			}
		}

		// Clear buffers
		for(auto buffer : buffers)
			delete buffer;

		return meshes;
	}

	//----------------------------------------------------------------------------------------------
	auto loadMaterials(
		const gltf::Document& _document,
		const shared_ptr<const Effect>& _pbrEffect,
		const std::vector<std::shared_ptr<Texture>>& _textures
		)
	{
		std::vector<std::shared_ptr<Material>> materials;
		
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
			materials.push_back(mat);
		}

		return materials;
	}

	//----------------------------------------------------------------------------------------------
	// TODO: This method assumes the texture is sRGB.
	// Instead, textures should be loaded on demand, when real color space info is available, or a first pass
	// should be performed on materials, marking textures with their corresponding color spaces
	auto loadTextures(const gltf::Document& _document)
	{
		vector<shared_ptr<Texture>> textures;
		textures.reserve(_document.textures.size());
		for(auto& textDesc : _document.textures)
		{
			// TODO: Use texture sampler information
			//auto& sampler = _document.samplers[textDesc.sampler];
			auto& image = _document.images[textDesc.source];
			textures.push_back(Texture::load(image.uri));
		}

		return textures;
	}

	//----------------------------------------------------------------------------------------------
	void loadGLTFScene(
		SceneNode& _parentNode,
		const std::string& _assetsFolder,
		const std::string& _fileName,
		graphics::RenderScene& _gfxWorld,
		graphics::GeometryPool& _geomPool)
	{
		// Open file
		auto fileName = _assetsFolder + _fileName + ".gltf";
		core::File sceneFile(fileName);
		if(!sceneFile.sizeInBytes()) {
			core::Log::error("Unable to find scene asset");
			return;
		}
		// Load gltf document
		gltf::Document document = gltf::detail::Create(
			Json::parse(sceneFile.bufferAsText()),
			{ _assetsFolder, {}});

		// Verify document is supported
		auto asset = document.asset;
		if(asset.empty()) {
			core::Log::error("Can't find asset descriptor");
			return;
		}
		if(asset.version != "2.0") {
			core::Log::error("Wrong format version. GLTF assets must be 2.0");
			return;
		}

		// Create default material
		auto pbrEffect = Effect::loadFromFile("metal-rough.fx");
		auto defaultMaterial = std::make_shared<Material>(pbrEffect);

		// Load resources
		auto textures = loadTextures(document);
		auto materials = loadMaterials(document, pbrEffect, textures);
		auto meshes = loadMeshes(_assetsFolder, document, materials);

		// Load nodes
		auto nodes = loadNodes(document, meshes, materials, defaultMaterial, _gfxWorld);

		// Return the right scene
		auto& displayScene = document.scenes[document.scene];
		for(auto nodeNdx : displayScene.nodes)
			_parentNode.addChild(nodes[nodeNdx]);
	}
}}
