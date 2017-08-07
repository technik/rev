//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
#include "player.h"
#include <core/platform/platformInfo.h>
#include <game/scene/sceneNode.h>
#include <game/scene/transform/objTransform.h>
#include <video/graphics/renderObj.h>
#include <video/window/window.h>
#include <game/scene/transform/spinner.h>
#include <game/scene/transform/flybySrc.h>
#include <cjson/json.h>

using namespace cjson;

using namespace rev::core;
#ifndef ANDROID
using namespace rev::input;
#endif
using namespace rev::game;
using namespace rev::math;
using namespace rev::net;
using namespace rev::video;

using namespace std;

namespace rev {

	Player::Player(const StartUpInfo& _platformInfo)
		: rev::App3d(_platformInfo)
	{
		processArgs(_platformInfo);
#ifdef OPENGL_45
		mRenderer.init(&driver3d());
#endif
#ifdef REV_USE_VULKAN
		mRenderer.init(window().frameBuffer()); // Configure renderer to render into the frame buffer
		createGlobalObjects();
		prepareFactories();
		loadSceneFromFile("data/vrScene.scn");
		initGameScene();
#endif
	}

	//----------------------------------------------------------------
	Player::~Player() {
		VkDevice device = GraphicsDriver::get().device();
	}

	//----------------------------------------------------------------
	void Player::processArgs(const StartUpInfo& _info) {
#ifdef ANDROID
		//mSceneName = "vrScene.scn";
#else
		if (_info.argC > 1) {
//			mSceneName = _info.argV[1];
		}
#endif
	}

	//----------------------------------------------------------------
	void Player::createGlobalObjects() {

	}

	void Player::registerFactory(const std::string& _type, ComponentFactory _f) {
		mFactories.insert(std::make_pair(_type,_f));
	}

	//----------------------------------------------------------------
	void Player::prepareFactories() {
		registerFactory("RenderObj", [this](const cjson::Json& _data, SceneNode& _owner){
			RenderObj* model = RenderObj::construct(_data, _owner);
			model->mesh()->sendBuffersToGPU();
			mRenderScene.objects.push_back(model);
			return nullptr;
		});
		registerFactory("Transform", ObjTransform::construct);
		//registerFactory")
	}

	//----------------------------------------------------------------
	void Player::loadSceneFromFile(const std::string& _fileName) {
		Json sceneData;
		ifstream fileStream(_fileName);
		sceneData.parse(fileStream);
		for (const auto& objectData : sceneData["objects"]) {
			SceneNode* obj = new SceneNode;
			mRootGameObjects.push_back(obj);
			if(objectData.contains("components"))
				for (const auto& c : objectData["components"])
				{
					const std::string& componentType = c["_type"];
					auto iter = mFactories.find(componentType);
					if (iter == mFactories.end())
					{
						cout << "Error: Unable to find factory for component of type " << componentType << "\n";
						continue;
					}
					Component* component = iter->second(c, *obj);
					if(component)
						obj->addComponent(component);
				}
			obj->init();
		}
	}

	//----------------------------------------------------------------
	void Player::initGameScene() {
		size_t rows = 3;
		size_t cols = 3;
		size_t nObjs = rows*cols;
		mRootGameObjects.reserve(nObjs);
		mRenderScene.objects.reserve(nObjs);
		// Light
		mRenderScene.lightClr = Vec3f(255.f/255.f, 51.f/255.f, 153.f/255.f);
		mRenderScene.lightDir = Vec3f(1.f, 0.f, 2.f);
		// Camera
		mRenderScene.camera = new Camera(60.f*3.14f/180.f, 0.1f, 100.f);
		SceneNode* camObj = new SceneNode(2);
		mRootGameObjects.push_back(camObj);
		ObjTransform *tr = new game::ObjTransform(*camObj);
		mRenderScene.camera->setTransform(tr);
		tr->setPosition({0.f, 0.f, 1.f});
		camObj->addComponent(tr);
		camObj->addComponent(new FlyBySrc(1.f, *camObj));
		camObj->init();
		// Scene
		// Castle
		/*SceneNode* castle = new SceneNode(2); // Reserve space for 2 components
		mRootGameObjects.push_back(castle);
		ObjTransform *castleTr = new game::ObjTransform(*castle);
		castle->addComponent(castleTr);
		castle->init();
		RenderGeom* castleMesh = RenderGeom::loadFromFile("data/sponzaLow.rmd");
		castleMesh->sendBuffersToGPU();
		mRenderScene.objects.push_back(new RenderObj(castleMesh, *castleTr));
		// Wheels
		RenderGeom* ballMesh = RenderGeom::loadFromFile("data/wheel.rmd");
		ballMesh->sendBuffersToGPU(); // So vulkan can render it
		float h = 2.f;
		float w = 2.f;
		float x0 = -w*0.5f;
		float z0 = 1.f-h*0.5f;
		for(size_t r = 0; r < rows; ++r) {
			float z = z0 + r*h/(rows-1);
			for(size_t c = 0; c < cols; ++c) {
				float x = x0 + c*w/(cols-1);
				SceneNode* obj = new SceneNode(2); // Reserve space for 2 components
				mRootGameObjects.push_back(obj);
				auto spinner = new Spinner(*obj);
				spinner->setSpin(Vec3f::zAxis());
				obj->addComponent(spinner);
				ObjTransform *tr = new game::ObjTransform(*obj);
				tr->setPosition({x, 4.f, z});
				obj->addComponent(tr);
				obj->init();
				mRenderScene.objects.push_back(new RenderObj(ballMesh, *tr));
			}
		}*/

	}

	//----------------------------------------------------------------
	bool Player::frame(float _dt) {
		t += _dt;
		for(auto obj : mRootGameObjects)
			obj->update();
		math::Mat44f projMtx = mRenderScene.camera->projection();
		math::Mat44f viewProj = projMtx*mRenderScene.camera->view().inverse();
		mRenderer.beginFrame();
		mRenderer.render(mRenderScene, viewProj);
		mRenderer.endFrame();

		return true;
		//return mGameWorld.update(_dt);
	}

}	// namespace rev