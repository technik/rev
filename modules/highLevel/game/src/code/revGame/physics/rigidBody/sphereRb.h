//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On 2013/July/18
//----------------------------------------------------------------------------------------------------------------------
// Physics rigid body

#ifndef _REV_GAME_PHYSICS_RIGIDBODY_SPHERERB_H_
#define _REV_GAME_PHYSICS_RIGIDBODY_SPHERERB_H_

#include "rigidBody.h"

namespace rev { namespace game {

	class SphereRb : public RigidBody{
	public:
		SphereRb(float _mass, float _radius);
	};
}	// namespace game
}	// namespace rev

#endif // _REV_GAME_PHYSICS_RIGIDBODY_SPHERERB_H_