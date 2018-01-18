// Immediate mode GUI for debug. Built on top of ImGUI https://github.com/ocornut/imgui
#pragma once

#include <math/algebra/vector.h>

namespace rev { namespace graphics { namespace gui {

	void init(const math::Vec2u& _windowSize);
	void startFrame();
	void finishFrame(float dt);

}}}