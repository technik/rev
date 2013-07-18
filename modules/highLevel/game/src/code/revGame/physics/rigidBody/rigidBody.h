//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On 2013/July/18
//----------------------------------------------------------------------------------------------------------------------
// Physics rigid body

#ifndef _REV_GAME_PHYSICS_RIGIDBODY_RIGIDBODY_H_
#define _REV_GAME_PHYSICS_RIGIDBODY_RIGIDBODY_H_

#include <bullet/btBulletDynamicsCommon.h>
#include <revMath/algebra/vector.h>
// #include <revGame/core/transform/sceneNode.h>

namespace rev { namespace game {

	class RigidBody {
	public:
		RigidBody(float _mass, const math::Vec3f& _inertia, btCollisionShape* _shape);
		virtual ~RigidBody();

		void applyImpulse(const math::Vec3f& _impulse);
		// Position is given in world coordinates, relative to the rigid body's COM
		void applyImpulse(const math::Vec3f& _impulse, const math::Vec3f& _position);
	};
}	// namespace game
}	// namespace rev

#endif // _REV_GAME_PHYSICS_RIGIDBODY_RIGIDBODY_H_