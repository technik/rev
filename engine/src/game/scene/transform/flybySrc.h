//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
//----------------------------------------------------------------------------------------------------------------------
// Keyboard controlled fly-by transform source

#ifndef _REV_GAME_SCENE_TRANSFORM_FLYBYSRC_H_
#define _REV_GAME_SCENE_TRANSFORM_FLYBYSRC_H_

#include <game/logicComponent.h>
#include <core/components/sceneNode.h>
#include <core/components/affineTransform.h>

namespace rev {
	namespace game {

		class FlyBySrc : public LogicComponent
		{
		public:
			FlyBySrc(float _spd) : mSpeed(_spd) {}

			void init() override {
				transform = node()->component<core::AffineTransform>();
			}

			bool update(float _dt) override;
			void setSpeed(float _speed) { mSpeed = _speed; }

		private:
			float mSpeed;
			core::AffineTransform*	transform = nullptr;
		};

	}	// namespace game
}	// namespace rev

#endif // _REV_GAME_SCENE_TRANSFORM_FLYBYSRC_H_