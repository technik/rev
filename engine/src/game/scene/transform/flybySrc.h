//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
//----------------------------------------------------------------------------------------------------------------------
// Keyboard controlled fly-by transform source

#ifndef _REV_GAME_SCENE_TRANSFORM_FLYBYSRC_H_
#define _REV_GAME_SCENE_TRANSFORM_FLYBYSRC_H_

#include <game/scene/sceneNode.h>
#include <core/components/affineTransform.h>

namespace rev {
	namespace game {

		class FlyBySrc : public Component
		{
		public:
			FlyBySrc(float _spd) : mSpeed(_spd) {}

			void init() override {
				transform = node().getComponent<core::AffineTransform>();
			}

			void update() override;
			void setSpeed(float _speed) { mSpeed = _speed; }

		private:
			float mSpeed;
			core::AffineTransform*	transform = nullptr;
		};

	}	// namespace game
}	// namespace rev

#endif // _REV_GAME_SCENE_TRANSFORM_FLYBYSRC_H_