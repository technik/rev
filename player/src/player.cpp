//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#define STB_IMAGE_IMPLEMENTATION
#include "player.h"
#include <math/algebra/vector.h>
#include <core/platform/fileSystem/file.h>
#include <core/time/time.h>
#include <core/tools/log.h>
#include <game/geometry/modelAsset.h>
#include <game/scene/camera.h>
#include <game/scene/meshRenderer.h>
#include <game/scene/transform/flyby.h>
#include <game/scene/transform/orbit.h>
#include <game/scene/transform/transform.h>
#include <graphics/debug/debugGUI.h>
#include <graphics/scene/material.h>
#include <graphics/scene/renderGeom.h>

using namespace rev::math;
using namespace rev::graphics;
using namespace rev::game;

using Json = rev::core::Json;

namespace rev {

	const std::vector<Vec3f> vertices = {
		{1.f, 0.f, 1.f},
		{-1.f, 0.f, 1.f},
		{0.f,0.f, -1.f}
	};
	const std::vector<uint16_t> indices = { 0, 1, 2};

	namespace { // Anonymous namespace for temporary implementation of gltf loader

		namespace gltf {
			struct Buffer
			{
				Buffer(const std::string& assetsFolder, const Json& desc)
				{
					size_t length = desc["byteLength"];
					raw.resize(length);
					core::File bufferFile(assetsFolder + desc["uri"].get<std::string>());
					// TODO: Error checking with buffer size
					memcpy(raw.data(), bufferFile.buffer(), length);
				}

				std::vector<uint8_t> raw;
			};

			struct BufferView
			{
				uint8_t* data = nullptr;
				GLenum target = 0;
				size_t size = 0;
				size_t stride = 0;
			};

			struct Accessor
			{
				std::string type;

				enum class ComponentType : uint32_t
				{
					BYTE = 5120,
					UNSIGNED_BYTE = 5121,
					SHORT = 5122,
					UNSIGNED_SHORT = 5123,
					UNSIGNED_INT = 5125,
					FLOAT = 5126
				} componentType;

				uint8_t* data = nullptr;
				size_t count = 0;
			};

			struct Primitive
			{
			public:
				Primitive(const std::vector<Accessor>& accessors, const Json& desc)
				{
					auto& attributes = desc["attributes"];
					position = &accessors[attributes["POSITION"]];
					normal = &accessors[attributes["NORMAL"]];
					tangent = &accessors[attributes["TANGENT"]];
					texCoord = &accessors[attributes["TEXCOORD_0"]];

					indices = &accessors[desc["indices"]];
					material = desc["material"];
				}

				const Accessor* position = nullptr;
				const Accessor* normal = nullptr;
				const Accessor* tangent = nullptr;
				const Accessor* texCoord = nullptr;

				const Accessor* indices = nullptr;

				size_t material;
			};

			struct Mesh
			{
				Mesh(const std::vector<Accessor>& accessors, const Json& desc)
				{
					for(auto& p : desc["primitives"])
						primitives.emplace_back(accessors, p);
				}

				std::vector<Primitive> primitives;
			};

		}

		std::shared_ptr<SceneNode> loadGLTFScene(const std::string& assetsFolder, graphics::RenderScene& _renderable)
		{
			Json sceneDesc;
			std::ifstream("helmet/damagedHelmet.gltf") >> sceneDesc;
			auto asset = sceneDesc.find("asset");
			if(asset == sceneDesc.end())
				return nullptr;
			if(asset.value()["version"] != "2.0")
				return nullptr;
			auto scene = sceneDesc.find("scene");
			auto scenesDict = sceneDesc.find("scenes");
			if(scenesDict == sceneDesc.end() || scene == sceneDesc.end() || scenesDict.value().size() == 0)
				return nullptr;
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
				unsigned byteOffset = 0;
				auto offsetIter = accessorDesc.find("byteOffset");
				if(offsetIter != accessorDesc.end())
					byteOffset = offsetIter.value().get<unsigned>();
				accessor.count = accessorDesc["count"];
				accessor.data = &bufferViews[bufferViewNdx].data[byteOffset];
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

			// Load materials
			std::vector<std::shared_ptr<graphics::Material>> materials;
			for(auto& matDesc : sceneDesc["materials"])
			{
				size_t albedoNdx = matDesc["pbrMetallicRoughness"]["baseColorTexture"]["index"];
				size_t physicsNdx = matDesc["pbrMetallicRoughness"]["metallicRoughnessTexture"]["index"];
				size_t emissiveNdx = matDesc["emissiveTexture"]["index"];
				size_t aoNdx = matDesc["occlusionTexture"]["index"];
				size_t normalNdx = matDesc["normalTexture"]["index"];
				auto mat = std::make_shared<Material>();
				mat->name = matDesc["name"].get<std::string>();
				mat->shader = "metal-rough.fx";
				mat->addTexture(5, Texture::load(textureNames[albedoNdx]));
				mat->addTexture(6, Texture::load(textureNames[physicsNdx]));
				mat->addTexture(7, Texture::load(textureNames[emissiveNdx]));
				mat->addTexture(8, Texture::load(textureNames[aoNdx]));
				mat->addTexture(9, Texture::load(textureNames[normalNdx], false));
				materials.push_back(mat);
			}

			// Load meshes
			std::vector<gltf::Mesh>	meshes;
			for(auto& meshDesc : sceneDesc["meshes"])
			{
				meshes.emplace_back(accessors, meshDesc);
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
				if(nodeDesc.find("matrix") != nodeDesc.end())
				{
					auto& matrixDesc = nodeDesc["matrix"];
					auto nodeTransform = std::make_unique<game::Transform>();
					for(size_t i = 0; i < 3; ++i)
						for(size_t j = 0; j < 4; ++j)
							nodeTransform->xForm.matrix()(i,j) = matrixDesc[i+4*j].get<float>();
					node->addComponent(std::move(nodeTransform));
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
							memcpy(indices.data(), primitive.indices->data, sizeof(uint16_t)*indices.size());
						}
						else
							return nullptr;

						// Copy vertex data
						std::vector<RenderGeom::Vertex> vertices(primitive.position->count);
						auto srcPosition = reinterpret_cast<math::Vec3f*>(primitive.position->data);
						auto srcNormal = reinterpret_cast<math::Vec3f*>(primitive.normal->data);
						auto srcTexCoord = reinterpret_cast<math::Vec2f*>(primitive.texCoord->data);
						auto srcTangent = reinterpret_cast<math::Vec4f*>(primitive.tangent->data);
						for(size_t i = 0; i < vertices.size(); ++i)
						{
							auto& v = vertices[i];
							v.position = srcPosition[i];
							v.normal = srcNormal[i];
							v.tangent = srcTangent[i].block<3,1>(0,0);
							v.bitangent = v.normal.cross(v.tangent)*srcTangent[i].w();
							v.uv = srcTexCoord[i];
						}
						
						// TODO: Share meshes
						renderObj->meshes.push_back(std::make_shared<RenderGeom>(vertices,indices));
						renderObj->materials.push_back(materials[0]); // TODO
					}

					auto nodeMesh = std::make_unique<game::MeshRenderer>(renderObj);
					_renderable.renderables().push_back(renderObj);

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
			auto& displayScene = scenesDict.value()[(unsigned)scene.value()];
			return nodes[displayScene["nodes"][0]];
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Player::init(Window _window) {
		core::Time::init();

		assert(!mGfxDriver);
		mGfxDriver = GraphicsDriverGL::createDriver(_window);
		if(mGfxDriver) {
			//loadScene("sponza_crytek");
			auto gltfScene = loadGLTFScene("helmet/", mGraphicsScene);
			//std::string skyName = "milkyway";
			std::string skyName = "Shiodome";
			//std::string skyName = "monument";
			//std::string skyName = "Ice";
			//std::string skyName = "Winter";
			//std::string skyName = "Factory";
			mGraphicsScene.sky = Texture::load(skyName+".hdr");
			mGraphicsScene.irradiance = Texture::load(skyName+"_irradiance.hdr");
			
			if(gltfScene)
			{
				auto orbitNode = std::make_shared<SceneNode>("orbit");
				orbitNode->addComponent<Transform>();
				orbitNode->addComponent<Orbit>(Vec2f{1e-2f,1e-2f});
				orbitNode->addChild(gltfScene);
				mGameScene.root()->addChild(orbitNode);
				auto xForm = gltfScene->component<Transform>();
				auto rotation = math::Mat33f({
					-1.f, 0.f, 0.f,
					0.f, 0.f, 1.f,
					0.f, 1.f, 0.f
					});
				if(!xForm)
				{
					xForm = gltfScene->addComponent<Transform>();
				}
				xForm->xForm.rotate(rotation);
			}
			createCamera();
			mGameScene.root()->init();

			mGameEditor.init(mGraphicsScene);
			mRenderer.init(*mGfxDriver, *mGfxDriver->frameBuffer());
			gui::init(_window->size);
		}
		return mGfxDriver != nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::onWindowResize(const math::Vec2u& _newSize)
	{
		if(mGfxDriver)
			mGfxDriver->onWindowResize(_newSize);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::createCamera() {
		// Orbit
		auto orbitNode = mGameScene.root()->createChild("camOrbit");
		orbitNode->addComponent<Orbit>(Vec2f{-1e-2f,-1e-2f}, 1);
		orbitNode->addComponent<Transform>();

		// Cam node
		auto cameraNode = orbitNode->createChild("Camera");
		cameraNode->addComponent<FlyBy>(1.f);
		cameraNode->addComponent<Transform>()->xForm.position() = math::Vec3f { 0.f, -4.f, 0.f };
		mCamera = &cameraNode->addComponent<game::Camera>()->cam();
	}

	struct MeshHeader
	{
		uint32_t nVertices;
		uint32_t nIndices;
	};

	struct RenderObjData {
		int meshIdx = -1;
		Mat44f transform;
	};

	namespace {
		struct MeshFactory
		{
			MeshFactory(
				const game::ModelAsset& meshPool,
				graphics::RenderScene& scene
			)
				: mMeshPool(meshPool)
				, mScene(scene)
			{}

			// TODO: Also use materials from the material manager, or some alternative way of loading materials
			// Factory signature: std::unique_ptr<Component>(const std::string&, std::istream&)
			std::unique_ptr<game::Component> operator()(const std::string&, std::istream& in) const
			{
				std::string sceneName, materialName;
				in >> sceneName;
				in.get();
				uint32_t nMeshes;
				in.read((char*)&nMeshes, sizeof(nMeshes));
				std::vector<uint32_t>  meshList(nMeshes);
				auto renderObj = std::make_shared<RenderObj>();
				for(auto& meshIdx : meshList)
				{
					in.read((char*)&meshIdx, sizeof(uint32_t));
					renderObj->meshes.push_back(mMeshPool.mesh(meshIdx));
					// TODO: Actually load materials
					in >> materialName;
					in.get();
					renderObj->materials.push_back(nullptr);
				}
				mScene.renderables().push_back(renderObj);
				return std::make_unique<MeshRenderer>(
					renderObj);
			}

		private:
			const game::ModelAsset& mMeshPool;
			graphics::RenderScene& mScene;
		};
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::loadScene(const std::string& _assetFileName)
	{
		// TODO: Use a real geometry pool. Even better. Use a geometry pool and a model mananger to load geometry.
		// Even better: Do that in a background thread while other components are loaded
		game::ModelAsset geometryPool(_assetFileName + ".mdl");
		// Load the scene components
		core::File asset(_assetFileName + ".scn");
		if(asset.sizeInBytes() == 0)
		{
			rev::core::Log::error("Unable to load asset");
			return;
		}
		auto& in = asset.asStream();
		mComponentFactory.registerFactory(
			"MeshRenderer",
			MeshFactory(geometryPool, mGraphicsScene),
			true);
		// Load scene nodes
		if(!mGameScene.load(in, mComponentFactory))
		{
			rev::core::Log::error("Error loading scene");
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Player::update()
	{
		if(!mGfxDriver)
			return true;
		core::Time::get()->update();
		gui::startFrame(mGfxDriver->frameBuffer()->size());
		mGameEditor.update(mGameScene);

		auto dt = core::Time::get()->frameTime();

		mGameScene.root()->update(dt);

		mRenderer.render(*mCamera, mGraphicsScene);

		gui::finishFrame(dt);
		mGfxDriver->swapBuffers();

		return true;
	}

}	// namespace rev