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

		initGameScene();
#endif
	}

	//----------------------------------------------------------------
	Player::~Player() {
		VkDevice device = driver3d().device();
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
	void Player::initGameScene() {
		size_t rows = 3;
		size_t cols = 3;
		size_t nObjs = rows*cols;
		mRootGameObjects.reserve(nObjs);
		mRenderScene.objects.reserve(nObjs);

		RenderGeom* ballMesh = RenderGeom::loadFromFile("data/wheel.rmd");
		ballMesh->sendBuffersToGPU(); // So vulkan can render it
		float h = 2.f;
		float w = 2.f;
		float x0 = -w*0.5f;
		float z0 = -h*0.5f;
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
				tr->setPosition({x, 2.f, z});
				obj->addComponent(tr);
				obj->init();
				mRenderScene.objects.push_back(new RenderObj(ballMesh, *tr));
			}
		}

	}

	//----------------------------------------------------------------
	bool Player::frame(float _dt) {
		t += _dt;
		mRenderer.beginFrame();
		for(auto obj : mRootGameObjects)
			obj->update();
		math::Mat44f projMtx = GraphicsDriver::projectionMtx(60.f*3.14f/180.f, 4.f/3.f,0.1f,10.f);
		math::Mat44f viewProj = projMtx;
		mRenderer.render(mRenderScene, viewProj);
		mRenderer.endFrame();

		return true;
		//return mGameWorld.update(_dt);
	}

}	// namespace rev