//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
#include "player.h"
#include <core/platform/platformInfo.h>
#include <video/window/window.h>

using namespace cjson;

using namespace rev::core;
#ifndef ANDROID
using namespace rev::input;
#endif
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

		VkDevice device = driver3d().device();
#endif
	}

	//----------------------------------------------------------------
	Player::~Player() {
		VkDevice device = driver3d().device();
	}

	//----------------------------------------------------------------
	void Player::processArgs(const StartUpInfo& _info) {
#ifdef ANDROID
		mSceneName = "vrScene.scn";
#else
		if (_info.argC > 1) {
//			mSceneName = _info.argV[1];
		}
#endif
	}

	//----------------------------------------------------------------
	bool Player::frame(float _dt) {
		mRenderer.beginFrame();
		math::Mat44f worldMtx = math::Mat44f::identity();
		mRenderer.render(mTriangle, worldMtx);
		mRenderer.endFrame();

		return true;
		//return mGameWorld.update(_dt);
	}

}	// namespace rev