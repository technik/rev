//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <graphics/driver/openGL/GraphicsDriverOpenGL.h>

namespace rev {

	class Player {
	public:
		typedef rev::graphics::GraphicsDriverGL::NativeWindow	Window;
		bool init(Window);

		// Common components
		bool update();

	private:
		graphics::GraphicsDriverGL* mGfxDriver = nullptr;
	};

}	// namespace rev