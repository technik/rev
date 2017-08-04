//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
#include "player.h"
#include <core/platform/platformInfo.h>
#include <game/scene/sceneNode.h>
#include <video/window/window.h>

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

		mBallObj = new SceneNode(1);
		ObjTransform *t = new game::ObjTransform(*mBallObj);
		t->setPosition({0.f, 1.f, 0.f});
		mBallObj->addComponent(t);
		mBallObj->init();
		RenderGeom* ballMesh = RenderGeom::loadFromFile("data/unitSphere.rmd");
		mBallGeom = new RenderObj(ballMesh, *t);
		mBallGeom->mesh()->sendBuffersToGPU(); // So vulkan can render it
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
	bool Player::frame(float _dt) {
		//t += _dt;
		mRenderer.beginFrame();
		mBallObj->update();
		math::Mat34f worldMtx = mBallGeom->transform();
		math::Mat44f projMtx = GraphicsDriver::projectionMtx(90.f*3.14f/180.f, 4.f/3.f,0.1f,10.f);
		math::Mat44f wvp = projMtx*worldMtx;
		mRenderer.render(*mBallGeom->mesh(), wvp);
		mRenderer.endFrame();

		return true;
		//return mGameWorld.update(_dt);
	}

}	// namespace rev