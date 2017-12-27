//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <graphics/driver/openGL/GraphicsDriverOpenGL.h>

namespace rev {

	class Player {
	public:
		typedef rev::graphics::GraphicsDriverGL::NativeWindow	Window;
		bool init(Window) { return true; }

		// Common components
		bool update() { return true; }
	};

}	// namespace rev