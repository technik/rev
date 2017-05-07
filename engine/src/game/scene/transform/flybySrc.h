//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
//----------------------------------------------------------------------------------------------------------------------
// Keyboard controlled fly-by transform source

#ifndef _REV_GAME_SCENE_TRANSFORM_FLYBYSRC_H_
#define _REV_GAME_SCENE_TRANSFORM_FLYBYSRC_H_

#include <core/components/transformSrc.h>

namespace rev {
	namespace game {

		class FlyBySrc : public core::TransformSrc
		{
		public:
			FlyBySrc(float _spd) : mSpeed(_spd) {}

			void setPosition(const math::Vec3f& _position);

			void update(float _dt);
			void setSpeed(float _speed) { mSpeed = _speed; }

		private:
			void			moveLocal(const math::Vec3f& _translation);
			void			rotate(const math::Vec3f& _axis, float _angle);

			float mSpeed;
		};

	}	// namespace game
}	// namespace rev

#endif // _REV_GAME_SCENE_TRANSFORM_FLYBYSRC_H_