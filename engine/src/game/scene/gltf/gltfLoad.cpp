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
#include <core/string_util.h>
#include <game/scene/transform/transform.h>
#include <game/scene/LightComponent.h>
#include <game/scene/meshRenderer.h>
#include <game/scene/camera.h>
#include <game/scene/transform/flyby.h>
#include <graphics/scene/renderGeom.h>
#include <graphics/scene/renderMesh.h>
#include <graphics/scene/renderObj.h>
#include <graphics/scene/animation/skinning.h>
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
	void loadSkin(
		const gltf::Document& document,
		const gltf::Skin& skinDesc,
		const vector<RenderGeom::Attribute>& attributes)
	{
		// Load skeleton
		auto& nodes = document.nodes;
		auto skeleton = std::make_shared<graphics::Skeleton>();
		auto numJoints = skinDesc.joints.size();
		// Initialize parent indices to -1
		skeleton->parentIndices.resize(numJoints, -1);
		// Traverse nodes to mark child nodes
		for(auto& joint : skinDesc.joints)
		{
			auto& jointNode = document.nodes[joint];
			for(int i = 0; i < numJoints; ++i)
			{
				for(auto& child : jointNode.children)
				{
					if(child == i)
					{
						skeleton->parentIndices[i] = joint;
						break;
					}
				}
			}
		}

		// Load skin's inverse binding matrices
		auto skin = make_shared<Skinning>();
		skin->inverseBinding.resize(numJoints);
		auto IBMs = attributes[skinDesc.inverseBindMatrices];
		for(auto i = 0; i < numJoints; ++i)
			skin->inverseBinding[i] = IBMs.get<math::Mat44f>(i);
	}

	//----------------------------------------------------------------------------------------------
	void loadSkins(const vector<RenderGeom::Attribute>& attributes, const gltf::Document& document)
	{
		for(auto& skin : document.skins)
		{
			loadSkin(document, skin, attributes);
		}
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

			// Optional skinning
			if(nodeDesc.skin >= 0)
			{
				//
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
				node->addComponent<FlyBy>(1.f, -0.4f);
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
	RenderGeom::Attribute* generateTangentSpace(
		const RenderGeom::Attribute* positions,
		const RenderGeom::Attribute* uvs,
		const RenderGeom::Attribute* normals,
		const RenderGeom::Attribute* indices)
	{
		if(uvs->nComponents != 2)
		{
			core::Log::error("Only UVs with 2 components are supported");
			return nullptr;
		}
		// Accessor data
		auto tangents = new RenderGeom::Attribute;
		tangents->componentType = (GLenum)gltf::Accessor::ComponentType::Float;
		tangents->count = uvs->count;
		tangents->nComponents = 4;
		tangents->normalized = false;
		tangents->offset = 0;
		tangents->stride = 0;
		// Buffer view
		tangents->bufferView = make_shared<RenderGeom::BufferView>();
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
			
			tangent3 = tangent3 - (tangent3.dot(normal) * normal); // Orthogonal tangent
			tangent3 = tangent3.normalized(); // Orthonormal tangent
			tangent = { tangent3.x(), tangent3.y(), tangent3.z(), signbit(-tangent.w()) ? -1.f : 1.f };
		}

		// TODO: Allocate buffer in graphics driver
		tanBv.vbo = graphics::GraphicsDriverGL::get()->allocateStaticBuffer(GL_ARRAY_BUFFER, tanBv.byteLength, tanBv.data);
		delete[] reinterpret_cast<const Vec4f*>(tanBv.data);
		delete[] indexData;
		tanBv.data = nullptr;

		return tangents;
	}

	//----------------------------------------------------------------------------------------------
	shared_ptr<RenderGeom> loadPrimitive(
		const gltf::Document& _document,
		const vector<RenderGeom::Attribute>& _attributes,
		const gltf::Primitive& _primitive,
		bool _needsTangentSpace)
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
		const RenderGeom::Attribute* weights = registerAttribute(_primitive, "WEIGHTS_0", _attributes);
		const RenderGeom::Attribute* joints = registerAttribute(_primitive, "JOINTS_0", _attributes);

		if(_needsTangentSpace && !tangents)
		{
			if(!normals || !uv0)
			{
				core::Log::error("Mesh requires tangent space but doesn't provide normals or uvs. Normal generation is not supported. Skipping primitive");
				return nullptr;
			}
			tangents = generateTangentSpace(position, uv0, normals, indices);
		}

		return std::make_shared<RenderGeom>(indices, position, normals, tangents, uv0, weights, joints);
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
		const vector<RenderGeom::Attribute>& attributes,
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
				auto material = _materials[primitive.material];
				bool needsTangentSpace = !_document.materials[primitive.material].normalTexture.empty();
				auto geometry = loadPrimitive(_document, attributes, primitive, needsTangentSpace);
				mesh->mPrimitives.emplace_back(geometry, material);
			}
		}

		return meshes;
	}

	//----------------------------------------------------------------------------------------------
	std::shared_ptr<Texture> getTexture(
		const std::string& assetsFolder,
		const gltf::Document& document,
		std::vector<std::shared_ptr<Texture>>& textures,
		int32_t index,
		bool sRGB)
	{
		auto& texture = textures[index];
		if(texture) // Already allocated, reuse
			return texture;

		// Not previously allocated, do it now
		auto textDesc = document.textures[index];
		auto& image = document.images[textDesc.source];
		texture = Texture::load(assetsFolder + image.uri, sRGB);

		return texture;
	}

	//----------------------------------------------------------------------------------------------
	auto loadMaterials(
		const std::string& _assetsFolder,
		const gltf::Document& _document,
		const shared_ptr<Effect>& _pbrEffect,
		std::vector<std::shared_ptr<Texture>>& _textures
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
					mat->addTexture("uBaseColorMap", getTexture(_assetsFolder, _document, _textures, albedoNdx, false));
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
					mat->addTexture("uPhysics", getTexture(_assetsFolder, _document, _textures, ndx, false));
				}
				if(pbrDesc.roughnessFactor != 1.f)
					mat->addParam("uRoughness", pbrDesc.roughnessFactor);
				if(pbrDesc.metallicFactor != 1.f)
					mat->addParam("uMetallic", pbrDesc.metallicFactor);
			}
			if(!matDesc.emissiveTexture.empty())
				mat->addTexture("uEmissive", getTexture(_assetsFolder, _document, _textures, matDesc.emissiveTexture.index, false));
			if(!matDesc.normalTexture.empty())
			{
				// TODO: Load normal map in linear space!!
				mat->addTexture("uNormalMap", getTexture(_assetsFolder, _document, _textures, matDesc.normalTexture.index, false));
			}
			materials.push_back(mat);
		}

		return materials;
	}

	//----------------------------------------------------------------------------------------------
	bool openAndValidateDocument(const std::string& fullPath, const std::string& folder, gltf::Document& document)
	{
		// Open file
		core::File sceneFile(fullPath);
		if(!sceneFile.sizeInBytes()) {
			core::Log::error("Unable to find scene asset");
			return false;
		}
		// Load gltf document
		document = gltf::detail::Create(
			Json::parse(sceneFile.bufferAsText()),
			{ folder, {}});

		// Verify document is supported
		auto asset = document.asset;
		if(asset.empty()) {
			core::Log::error("Can't find asset descriptor");
			return false;
		}
		if(asset.version != "2.0") {
			core::Log::error("Wrong format version. GLTF assets must be 2.0");
			return false;
		}
		return true;
	}

	//----------------------------------------------------------------------------------------------
	void loadGLTFScene(
		SceneNode& _parentNode,
		const std::string& _filePath,
		graphics::RenderScene& _gfxWorld,
		graphics::GeometryPool& _geomPool)
	{
		// Open file
		auto folder = core::getPathFolder(_filePath);
		// Load gltf document
		gltf::Document document;
		if(!openAndValidateDocument(_filePath, folder, document))
			return;

		// Create default material
		auto pbrEffect = std::make_shared<Effect>("metal-rough.fx");
		auto defaultMaterial = std::make_shared<Material>(pbrEffect);

		// Load buffers
		vector<core::File*> buffers;
		for(auto b : document.buffers)
			buffers.push_back(new core::File(folder+b.uri));
		auto bufferViews = loadBufferViews(document, buffers); // // Load buffer views
		auto attributes = readAttributes(document, bufferViews); // Load accessors

		// Load resources
		loadSkins(attributes, document);
		std::vector<std::shared_ptr<Texture>> textures(document.textures.size(), nullptr);
		auto materials = loadMaterials(folder, document, pbrEffect, textures);
		auto meshes = loadMeshes(attributes, document, materials);

		// Load nodes
		auto nodes = loadNodes(document, meshes, materials, defaultMaterial, _gfxWorld);

		// Return the right scene
		auto& displayScene = document.scenes[document.scene];
		for(auto nodeNdx : displayScene.nodes)
			_parentNode.addChild(nodes[nodeNdx]);
	}
}}
