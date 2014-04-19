//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/19
//----------------------------------------------------------------------------------------------------------------------
// Platformer game level
#ifndef _PLATFORMER_LEVEL_GAMELEVEL_H_
#define _PLATFORMER_LEVEL_GAMELEVEL_H_

#include <core/containers/vector.h>

#include <video/graphics/renderer/forward/forwardRenderer.h>
#include <video/graphics/renderer/renderObj.h>

namespace platformer {

	class GameLevel {
	public:
		GameLevel();
		~GameLevel();
		void render(rev::video::ForwardRenderer*);
		void save(const char* _fileName) const;

	private:
		rev::core::vector<rev::video::RenderObj*>	mStaticGeomtry;
	};

}	// namespace platformer

#endif // _PLATFORMER_LEVEL_GAMELEVEL_H_