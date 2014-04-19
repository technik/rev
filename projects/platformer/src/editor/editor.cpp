//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/19
//----------------------------------------------------------------------------------------------------------------------
// In-game editor

#include "editor.h"

#include <game/geometry/procedural/basic.h>
#include <input/keyboard/keyboardInput.h>

using namespace rev::game;
using namespace rev::input;
using namespace rev::math;
using namespace rev::video;

namespace platformer {

	//------------------------------------------------------------------------------------------------------------------
	Editor::Editor(GameLevel* _level)
		:mMode(EditMode::play)
		,mLevel(_level)
	{
		createWidgets();
	}

	//------------------------------------------------------------------------------------------------------------------
	Editor::~Editor(){

	}

	//------------------------------------------------------------------------------------------------------------------
	void Editor::update(float) {
		KeyboardInput* keyboard = KeyboardInput::get();
		if (keyboard->pressed(KeyboardInput::eZ))
			mMode = EditMode::move;
		if (keyboard->pressed(KeyboardInput::eV))
			mMode = EditMode::play;
		// Update widget
		switch (mMode) {
		case EditMode::move:
			mWidget = mMove;
			break;
		default:
			mWidget = nullptr;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void Editor::render(ForwardRenderer* _renderer) {
		if (mWidget)
			_renderer->renderObject(*mWidget);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Editor::createWidgets() {
		mMove = Procedural::box(Vec3f(1.f, 0.1f, 0.1f));
		mMove->transform = Mat34f::identity();
	}

}	// namespace platformer