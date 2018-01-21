// Immediate mode GUI for debug. Built on top of ImGUI https://github.com/ocornut/imgui
#pragma once

#include <functional>
#include <math/algebra/vector.h>

namespace rev { namespace graphics { namespace gui {

	void init(const math::Vec2u& _windowSize);
	void startFrame();
	void finishFrame(float dt);

	bool beginWindow(const char* _name);
	void endWindow();

	void showCombo(const char* title, int& selected, std::function<const char*(size_t)> items, size_t nItems);
	void showList(const char* title, int& selected, std::function<const char*(size_t)> items, size_t nItems);

	void text(const char*);

}}}