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
#include <game/scene/camera.h>
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

			// Optional camera
			if(nodeDesc.camera >= 0)
			{
				if(nodeDesc.mesh >= 0 || nodeDesc.children.size() > 0)
				{
					cout << "Error: Cameras are only supported in separate nodes with no children\n";
				}
				auto& cam = _document.cameras[nodeDesc.camera];
				node->addComponent<game::Camera>(&_gfxWorld, cam.perspective.yfov, cam.perspective.znear, cam.perspective.zfar);
				// Camera correction matrix
				auto correction = Mat44f::identity();
				correction.block<3,1>(0,1) = Vec3f(0.f,0.f,1.f);
				correction.block<3,1>(0,2) = Vec3f(0.f,-1.f,0.f);
				node->component<Transform>()->xForm.rotate(Quatf({1.f,0.f,0.f}, -Constants<float>::halfPi));
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
	auto readAttributes(
		const gltf::Document& _document,
		const vector<shared_ptr<RenderGeom::BufferView>>& _bufferViews)
	{
		vector<RenderGeom::Attribute> attributes(_document.accessors.size());

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
		}

		return attributes;
	}

	//----------------------------------------------------------------------------------------------
	const RenderGeom::Attribute* registerAttribute(
		const gltf::Primitive& _primitive,
		const char* tag,
		const vector<RenderGeom::Attribute>& _attributes)
	{
		if(auto posIt = _primitive.attributes.find(tag); posIt != _primitive.attributes.end())
		{
			auto& attribute = _attributes[posIt->second];
			auto& bv = *attribute.bufferView;
		
			if(!bv.vbo)
				bv.vbo = GraphicsDriverGL::get()->allocateStaticBuffer(GL_ARRAY_BUFFER, bv.byteLength, bv.data);

			return &attribute;
		}

		return nullptr;
	}

	//----------------------------------------------------------------------------------------------
	shared_ptr<RenderGeom> loadPrimitive(
		const gltf::Document& _document,
		const vector<RenderGeom::Attribute>& _attributes,
		const gltf::Primitive& _primitive)
	{
		// Early out for invalid data
		if(_primitive.indices < 0)
			return nullptr;

		const RenderGeom::Attribute* indices = &_attributes[_primitive.indices];
		if(!indices->bufferView->vbo)
		{
			auto& bv = *indices->bufferView;
			indices->bufferView->vbo = GraphicsDriverGL::get()->allocateStaticBuffer(GL_ELEMENT_ARRAY_BUFFER, bv.byteLength, bv.data);
		}

		// Read primitive attributes
		const RenderGeom::Attribute* position = registerAttribute(_primitive, "POSITION", _attributes);
		const RenderGeom::Attribute* normals = registerAttribute(_primitive, "NORMAL", _attributes);
		const RenderGeom::Attribute* tangents = registerAttribute(_primitive, "TANGENT", _attributes);
		const RenderGeom::Attribute* uv0 = registerAttribute(_primitive, "TEXCOORD_0", _attributes);

		return std::make_shared<RenderGeom>(indices, position, normals, tangents, uv0);
	}

	//----------------------------------------------------------------------------------------------
	auto loadBufferViews(const gltf::Document& _document, const vector<core::File*>& buffers)
	{
		vector<std::shared_ptr<RenderGeom::BufferView>> bvs;

		for(auto& bv : _document.bufferViews)
		{
			auto bufferData = reinterpret_cast<const uint8_t*>(buffers[bv.buffer]->buffer());

			RenderGeom::BufferView bufferView;
			bufferView.vbo = 0;
			bufferView.byteStride = (GLint)bv.byteStride;
			bufferView.data = &bufferData[bv.byteOffset];
			bufferView.byteLength = bv.byteLength;

			bvs.push_back( make_shared<RenderGeom::BufferView>(bufferView) );
		}

		return bvs;
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
		auto bufferViews = loadBufferViews(_document, buffers); // // Load buffer views
		auto attributes = readAttributes(_document, bufferViews); // Load accessors

		// Load the meshes
		vector<shared_ptr<RenderMesh>> meshes;
		meshes.reserve(_document.meshes.size());
		for(auto& meshDesc : _document.meshes)
		{
			meshes.push_back(make_shared<RenderMesh>());
			auto mesh = meshes.back();
			for(auto& primitive : meshDesc.primitives)
			{
				auto material = _materials[primitive.material];
				auto geometry = loadPrimitive(_document, attributes, primitive);
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
	auto loadTextures(const std::string& _assetsFolder, const gltf::Document& _document)
	{
		vector<shared_ptr<Texture>> textures;
		textures.reserve(_document.textures.size());
		for(auto& textDesc : _document.textures)
		{
			// TODO: Use texture sampler information
			//auto& sampler = _document.samplers[textDesc.sampler];
			auto& image = _document.images[textDesc.source];
			textures.push_back(Texture::load(_assetsFolder + image.uri));
		}

		return textures;
	}

	//--------------------------------------------------------------------------------------------------
	inline string getFolder(const string& path)
	{
		auto pos = path.find_last_of("\\/");
		if (pos != string::npos)
			return path.substr(0, pos+1);

		return "";
	}

	//----------------------------------------------------------------------------------------------
	void loadGLTFScene(
		SceneNode& _parentNode,
		const std::string& _filePath,
		graphics::RenderScene& _gfxWorld,
		graphics::GeometryPool& _geomPool)
	{
		// Open file
		auto folder = getFolder(_filePath);
		core::File sceneFile(_filePath);
		if(!sceneFile.sizeInBytes()) {
			core::Log::error("Unable to find scene asset");
			return;
		}
		// Load gltf document
		gltf::Document document = gltf::detail::Create(
			Json::parse(sceneFile.bufferAsText()),
			{ folder, {}});

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
		auto textures = loadTextures(folder, document);
		auto materials = loadMaterials(document, pbrEffect, textures);
		auto meshes = loadMeshes(folder, document, materials);

		// Load nodes
		auto nodes = loadNodes(document, meshes, materials, defaultMaterial, _gfxWorld);

		// Return the right scene
		auto& displayScene = document.scenes[document.scene];
		for(auto nodeNdx : displayScene.nodes)
			_parentNode.addChild(nodes[nodeNdx]);
	}
}}
