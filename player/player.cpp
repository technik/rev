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
		if(mGfxDriver) {
			glClearColor(0.f,0.5f,1.f,1.f);
		}
		return mGfxDriver != nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Player::update() {
		if(!mGfxDriver)
			return true;
		
		glClear(GL_COLOR_BUFFER_BIT);
		mGfxDriver->swapBuffers();
		return true;
	}

}	// namespace rev