//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
#include "player.h"
#include <core/platform/platformInfo.h>

using namespace cjson;

using namespace rev::core;
using namespace rev::game;
#ifndef ANDROID
using namespace rev::input;
#endif
using namespace rev::math;
using namespace rev::net;
using namespace rev::video;

using namespace std;

namespace rev {


	//----------------------------------------------------------------
	bool Player::frame(float _dt) {
		mRenderer.render();
		return true;
	}

}	// namespace rev