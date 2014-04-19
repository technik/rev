//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/19
//----------------------------------------------------------------------------------------------------------------------
// In-game editor
#ifndef _PLATFORMER_EDITOR_EDITOR_H_
#define _PLATFORMER_EDITOR_EDITOR_H_

#include <video/graphics/renderer/forward/forwardRenderer.h>
#include <video/graphics/renderer/renderObj.h>

namespace platformer {

	class GameLevel;

	class Editor {
	public:
		Editor(GameLevel* _level);
		~Editor();
		void update(float _dt);
		void render(rev::video::ForwardRenderer*);

	private:
		void createGrid();
		void createWidgets();

	private:
		enum class EditMode {
			play,
			move,
			rotate,
			scale
		} mMode;

		GameLevel* mLevel;

		// Widgets
		rev::video::RenderObj*	mWidget = nullptr;
		rev::video::RenderObj*	mMove;

	};

}	// namespace platformer

#endif // _PLATFORMER_EDITOR_EDITOR_H_