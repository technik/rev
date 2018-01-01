//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#include "player.h"

namespace rev {

	//------------------------------------------------------------------------------------------------------------------
	bool Player::init(Window _window) {
		assert(!mGfxDriver);

		mGfxDriver = graphics::GraphicsDriverGL::createDriver(_window);
		return mGfxDriver != nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Player::update() {
		if(!mGfxDriver)
			return true;

		mGfxDriver->swapBuffers();
		return true;
	}

}	// namespace rev