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
		registerFactory("Spinner", [this](const cjson::Json& _data, SceneNode& _owner){
			Spinner* spinner = new Spinner(_owner);
			if(_data.contains("rotation")) {
				const cjson::Json& spin = _data["rotation"];
				spinner->setSpin(math::Vec3f(spin(0), spin(1), spin(2)));
			}
			return spinner;
		});
		registerFactory("Camera", [this](const cjson::Json& _data, SceneNode& _owner){
			Camera* cam = Camera::construct(_data);
			cam->setTransform(_owner.getComponent<ObjTransform>());
			mRenderScene.camera = cam;
			return nullptr;
		});
		registerFactory("FlyBy", [this](const cjson::Json& _data, SceneNode& _owner){
			float speed = 1.f;
			if(_data.contains("speed"))
				speed = _data["speed"];
			return new FlyBySrc(speed, _owner);
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
		// Light
		mRenderScene.lightClr = Vec3f(255.f/255.f, 51.f/255.f, 153.f/255.f);
		mRenderScene.lightDir = Vec3f(1.f, 0.f, 2.f);
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