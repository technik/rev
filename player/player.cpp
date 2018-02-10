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
#include <game/scene/camera.h>
#include <game/scene/meshRenderer.h>
#include <game/scene/transform/transform.h>
#include <graphics/debug/debugGUI.h>
#include <graphics/scene/material.h>

using namespace rev::math;
using namespace rev::graphics;
using namespace rev::game;

namespace rev {

	const std::vector<Vec3f> vertices = {
		{1.f, 0.f, 1.f},
		{-1.f, 0.f, 1.f},
		{0.f,0.f, -1.f}
	};
	const std::vector<uint16_t> indices = { 0, 1, 2};

	std::shared_ptr<Texture> redCurtainAlbedo;

	//------------------------------------------------------------------------------------------------------------------
	bool Player::init(Window _window) {
		core::Time::init();

		assert(!mGfxDriver);
		mGfxDriver = GraphicsDriverGL::createDriver(_window);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		if(mGfxDriver) {
			// -- triangle --
			mGameProject.load("sample.prj");
			// Create texture first to be able to use it during scene loading
			registerFactories();
			loadScene("sponza_crytek.scn");
			createCamera();

			// Register phony materials
			auto redCurtain = std::make_shared<Material>();
			redCurtain->name = "Red curtain";
			redCurtainAlbedo = Texture::load("textures/sponza_curtain_diff.tga");
			redCurtain->addTexture(5, redCurtainAlbedo->glName()); // Albedo texture
			redCurtain->addParam(6, 0.8f); // Roughness
			redCurtain->addParam(7, 0.05f); // Metallic
			mGraphicsScene.registerMaterial(redCurtain);

			mGameEditor.init(mGraphicsScene);
			mRenderer.init(*mGfxDriver);
			gui::init(_window->size);
		}
		return mGfxDriver != nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::registerFactories() {
		// Factory signature: std::unique_ptr<Component>(const std::string&, std::istream&)
		mComponentFactory.registerFactory("MeshRenderer", [this](const std::string&, std::istream& in)
		{
			uint32_t nMeshes;
			in.read((char*)&nMeshes, sizeof(nMeshes));
			std::vector<std::pair<uint32_t,uint32_t>>  meshList(nMeshes);
			for(auto& mesh : meshList)
			{
				in.read((char*)&mesh, 2*sizeof(size_t));
			}
			return std::make_unique<MeshRenderer>( mGraphicsScene.createRenderObj(meshList) );
		}, true);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::createCamera() {
		// Node
		auto cameraNode = std::make_shared<SceneNode>();
		mGameScene.root()->addChild(cameraNode);
		cameraNode->name = "Camera";
		// Transform
		auto objXForm = std::make_unique<Transform>();
		objXForm->matrix().setIdentity();
		objXForm->xForm.position() = math::Vec3f { 400.f, 120.f, 170.f };
		objXForm->xForm.setRotation(math::Quatf(Vec3f(0.f,0.f,1.f), 1.57f));
		cameraNode->addComponent(std::move(objXForm));
		// Actual camera
		auto camComponent = std::make_unique<game::Camera>();
		mCamera = &camComponent->cam();
		cameraNode->addComponent(std::move(camComponent));
		// Init camera
		cameraNode->init();
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

	//------------------------------------------------------------------------------------------------------------------
	void Player::loadScene(const char* _assetFileName)
	{
		core::File asset(_assetFileName);
		if(asset.sizeInBytes() == 0)
		{
			rev::core::Log::error("Unable to load asset");
			return;
		}
		auto& in = asset.asStream();
		// Load meshes
		uint32_t nMeshes;
		in.read((char*)&nMeshes, sizeof(nMeshes));
		for(uint32_t i = 0; i < nMeshes; ++i)
		{
			auto geom = std::make_shared<RenderGeom>();
			geom->deserialize(in);
			mGraphicsScene.registerMesh(geom);
		}
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
		gui::startFrame();
		mGameEditor.update(mGameScene);

		auto dt = core::Time::get()->frameTime();

		mGameScene.root()->update(dt);

		mRenderer.render(*mCamera, mGraphicsScene);

		gui::finishFrame(dt);
		mGfxDriver->swapBuffers();

		return true;
	}

}	// namespace rev