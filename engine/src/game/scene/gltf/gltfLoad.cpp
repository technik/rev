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
#include <core/platform/fileSystem/file.h>
#include <core/tools/log.h>
#include <core/string_util.h>
#include <nlohmann/json.hpp>
#include <game/animation/skeleton.h>
#include <game/resources/load.h>
#include <game/scene/transform/transform.h>
#include <game/scene/LightComponent.h>
#include <game/scene/meshRenderer.h>
#include <game/scene/meshRenderer.h>
#include <game/scene/camera.h>
#include <game/scene/transform/flyby.h>
#include <graphics/backend/device.h>
#include <graphics/scene/renderGeom.h>
#include <graphics/scene/renderMesh.h>
#include <graphics/scene/renderObj.h>
#include <graphics/scene/animation/skinning.h>
#include <graphics/renderer/material/Effect.h>
#include <graphics/renderer/material/material.h>
#include <vector>

using Json = nlohmann::json;

using namespace fx;
using namespace rev::gfx;
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
	auto loadNodes(
		const gltf::Document& _document,
		const vector<shared_ptr<RenderMesh>>& _meshes,
		const vector<shared_ptr<SkinInstance>>& _skins,
		const vector<shared_ptr<Material>>& _materials,
		shared_ptr<Material> _defaultMaterial,
		gfx::RenderScene& _gfxWorld)
	{
		vector<shared_ptr<SceneNode>> nodes;
		for(auto& nodeDesc : _document.nodes)
		{
			auto node = std::make_shared<SceneNode>();
			nodes.push_back(node);

			node->name = nodeDesc.name; // Node name

			// Optional node transform
			auto nodeTransform = loadNodeTransform(nodeDesc);
			if(nodeTransform)
				node->addComponent(std::move(nodeTransform));
		}

		// Build hierarchy
		size_t i = 0;
		for(auto& nodeDesc : _document.nodes)
		{
			auto& node = nodes[i++];
			for(auto& child : nodeDesc.children)
				node->addChild(nodes[child]);
		}

		i = 0;
		// Create node and add basic components
		for(auto& nodeDesc : _document.nodes)
		{
			auto& node = nodes[i++];
			
			// Optional node mesh
			std::shared_ptr<RenderObj> renderObj;
			if(nodeDesc.mesh >= 0)
			{
				renderObj = make_shared<RenderObj>(_meshes[nodeDesc.mesh]);
				node->addComponent<MeshRenderer>(renderObj);
				_gfxWorld.renderables().push_back(renderObj);
			}

			// Optional skinning
			if(nodeDesc.skin >= 0)
			{
				renderObj->skin = _skins[nodeDesc.skin];
			}

			// Optional camera
			if(nodeDesc.camera >= 0)
			{
				if(nodeDesc.mesh >= 0 || nodeDesc.children.size() > 0)
				{
					cout << "Error: Cameras are only supported in separate nodes with no children\n";
				}
				auto& cam = _document.cameras[nodeDesc.camera];
				auto camComponent = node->addComponent<game::Camera>(cam.perspective.yfov, cam.perspective.znear, cam.perspective.zfar);
				_gfxWorld.addCamera(camComponent->cam());
				// Camera correction matrix
				auto correction = Mat44f::identity();
				correction.block<3,1>(0,1) = Vec3f(0.f,0.f,1.f);
				correction.block<3,1>(0,2) = Vec3f(0.f,-1.f,0.f);
				node->addComponent<FlyBy>(1.f, -0.4f);

			}
		}

		return nodes;
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
		
			if(!bv.vbo.isValid())
				bv.vbo = device.allocateStaticVtxBuffer(bv.byteLength, bv.data);

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
		tanBv.vbo = device.allocateStaticVtxBuffer(tanBv.byteLength, tanBv.data);
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
			indices->bufferView->vbo = device.allocateIndexBuffer(bv.byteLength, bv.data);
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
	auto loadMeshes(
		gfx::Device& device,
		const vector<gfx::RenderGeom::Attribute>& attributes,
		const gltf::Document& _document,
		const vector<shared_ptr<Material>>& _materials,
		const shared_ptr<Material>& _defaultMaterial)
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
				auto material = _defaultMaterial;
				bool needsTangentSpace = false;
				if(primitive.material >= 0)
				{
					material = _materials[primitive.material];
					needsTangentSpace = !_document.materials[primitive.material].normalTexture.empty();
				}
				auto geometry = loadPrimitive(device, _document, attributes, primitive, needsTangentSpace);
				mesh->mPrimitives.emplace_back(geometry, material);
			}
			mesh->updateBBox();
		}

		return meshes;
	}

	//----------------------------------------------------------------------------------------------
	gfx::Texture2d getTexture(
		gfx::Device& device,
		const std::string& assetsFolder,
		const gltf::Document& document,
		std::vector<gfx::Texture2d>& textures,
		gfx::TextureSampler defaultSampler,
		int32_t index,
		bool sRGB)
	{
		auto& texture = textures[index];
		if(texture.isValid()) // Already allocated, reuse
			return texture;

		// Not previously allocated, do it now
		auto textGltfDesc = document.textures[index];
		auto& imageDesc = document.images[textGltfDesc.source];
		if(textGltfDesc.sampler >= 0)
		{
			// TODO: Support custom samplers
			//auto gltfSampler = document.samplers[textGltfDesc.sampler];
			//if(gltfSampler.wrapS == gltf::Sampler::WrappingMode::Repeat
		}
		texture = game::load2dTextureFromFile(device, defaultSampler, assetsFolder + imageDesc.uri, sRGB);

		return texture;
	}

	//----------------------------------------------------------------------------------------------
	auto loadMaterials(
		gfx::Device& gfxDevice,
		const std::string& _assetsFolder,
		const gltf::Document& _document,
		const shared_ptr<Effect>& _pbrEffect,
		const shared_ptr<Effect>& _specEffect,
		std::vector<gfx::Texture2d>& _textures
		)
	{
		std::vector<std::shared_ptr<Material>> materials;
		gfx::TextureSampler::Descriptor samplerDesc;
		samplerDesc.wrapS = gfx::TextureSampler::Descriptor::Wrap::Clamp;
		samplerDesc.wrapT = gfx::TextureSampler::Descriptor::Wrap::Clamp;
		auto sampler = gfxDevice.createTextureSampler(samplerDesc);
		auto envBRDF = load2dTextureFromFile(gfxDevice, sampler, "shaders/ibl_brdf.hdr", false, 1);

		// Create default texture sampler
		samplerDesc.wrapS = gfx::TextureSampler::Descriptor::Wrap::Repeat;
		samplerDesc.wrapT = gfx::TextureSampler::Descriptor::Wrap::Repeat;
		auto defSampler = gfxDevice.createTextureSampler(samplerDesc);

		auto clearCoatEffect = std::make_shared<Effect>("shaders/clearCoat.fx");
		auto clearCoat = std::make_shared<Material>(clearCoatEffect);
		
		// Load materials
		for(auto& matDesc : _document.materials)
		{
			std::shared_ptr<Material> mat;
			bool specular = false;
			if(matDesc.extensionsAndExtras.find("extensions") != matDesc.extensionsAndExtras.end())
			{
				auto& extensions = matDesc.extensionsAndExtras["extensions"];
				if(extensions.find("KHR_materials_pbrSpecularGlossiness") != extensions.end())
					specular = true;
			}
			if(specular)
				mat = std::make_shared<Material>(_specEffect);
			else
				mat = std::make_shared<Material>(_pbrEffect);
			if(matDesc.name == "carPaint")
				mat = clearCoat;
			auto& pbrDesc = matDesc.pbrMetallicRoughness;
			if(!pbrDesc.empty())
			{
				// Base color
				if(!pbrDesc.baseColorTexture.empty())
				{
					auto albedoNdx = pbrDesc.baseColorTexture.index;
					mat->addTexture("uBaseColorMap", getTexture(gfxDevice, _assetsFolder, _document, _textures, defSampler, albedoNdx, false));
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
					mat->addTexture("uPhysics", getTexture(gfxDevice, _assetsFolder, _document, _textures, defSampler, ndx, false));
				}
				if(pbrDesc.roughnessFactor != 1.f)
					mat->addParam("uRoughness", pbrDesc.roughnessFactor);
				if(pbrDesc.metallicFactor != 1.f)
					mat->addParam("uMetallic", pbrDesc.metallicFactor);
			}
			if(!matDesc.emissiveTexture.empty())
				mat->addTexture("uEmissive", getTexture(gfxDevice, _assetsFolder, _document, _textures, defSampler, matDesc.emissiveTexture.index, false));
			if(!matDesc.normalTexture.empty())
			{
				// TODO: Load normal map in linear space!!
				mat->addTexture("uNormalMap", getTexture(gfxDevice, _assetsFolder, _document, _textures, defSampler, matDesc.normalTexture.index, false));
			}
			mat->addTexture("uEnvBRDF", envBRDF);
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
		auto jsonText = sceneFile.bufferAsText();
		document = gltf::detail::Create(
			Json::parse(jsonText, nullptr, false),
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

	//----------------------------------------------------------------------------------------------
	void loadGLTFScene(
		gfx::Device& gfxDevice,
		SceneNode& _parentNode,
		const std::string& _filePath,
		gfx::RenderScene& _gfxWorld,
		std::vector<std::shared_ptr<SceneNode>>& animNodes,
		vector<shared_ptr<Animation>>& _animations)
	{
		// Open file
		auto folder = core::getPathFolder(_filePath);
		// Load gltf document
		gltf::Document document;
		if(!openAndValidateDocument(_filePath, folder, document))
			return;

		// Create default material
		auto pbrEffect = std::make_shared<Effect>("shaders/metal-rough.fx");
		auto specEffect = std::make_shared<Effect>("shaders/specularSetup.fx");
		auto defaultMaterial = std::make_shared<Material>(pbrEffect);

		gfx::TextureSampler::Descriptor samplerDesc;
		samplerDesc.wrapS = gfx::TextureSampler::Descriptor::Wrap::Clamp;
		samplerDesc.wrapT = gfx::TextureSampler::Descriptor::Wrap::Clamp;
		auto sampler = gfxDevice.createTextureSampler(samplerDesc);
		auto envBRDF = load2dTextureFromFile(gfxDevice, sampler, "shaders/ibl_brdf.hdr", false, 1);
		defaultMaterial->addTexture("uEnvBRDF", envBRDF);

		// Load buffers
		vector<core::File*> buffers;
		for(auto b : document.buffers)
			buffers.push_back(new core::File(folder+b.uri));
		auto bufferViews = loadBufferViews(document, buffers); // // Load buffer views
		auto attributes = readAttributes(document, bufferViews); // Load accessors

		// Load resources
		auto skins = loadSkins(attributes, document);
		std::vector<gfx::Texture2d> textures(document.textures.size());
		auto materials = loadMaterials(gfxDevice, folder, document, pbrEffect, specEffect, textures);
		auto meshes = loadMeshes(gfxDevice, attributes, document, materials, defaultMaterial);

		// Load nodes
		auto nodes = loadNodes(document, meshes, skins, materials, defaultMaterial, _gfxWorld);

		// Load animations
		loadAnimations(document, attributes, nodes, animNodes, _animations);

		// Return the right scene
		int sceneIndex = document.scene == -1 ? 0 : document.scene;
		auto& displayScene = document.scenes[sceneIndex];
		for(auto nodeNdx : displayScene.nodes)
			_parentNode.addChild(nodes[nodeNdx]);
	}
}}
