//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/19
//----------------------------------------------------------------------------------------------------------------------
// Platformer game level
#include "gameLevel.h"

#include <game/geometry/procedural/basic.h>
#include <game/scene/sceneGraph/sceneNode.h>

using namespace rev::game;
using namespace rev::math;
using namespace rev::video;

namespace platformer {

	//------------------------------------------------------------------------------------------------------------------
	GameLevel::GameLevel() {
		RenderObj* floor = Procedural::box(Vec3f(1000.f, 1000.f, 1.f));
		SceneNode auxNode;
		auxNode.setPos(Vec3f::zAxis()*-0.5f);
		floor->transform = auxNode.transform();
		mStaticGeomtry.push_back(floor);
	}

	//------------------------------------------------------------------------------------------------------------------
	GameLevel::~GameLevel() {
		// TODO clean uo memory
	}

	//------------------------------------------------------------------------------------------------------------------
	void GameLevel::render(ForwardRenderer* _renderer) {
		for (auto obj : mStaticGeomtry) {
			_renderer->renderObject(*obj);
		}
	}

}	// namespace platformer