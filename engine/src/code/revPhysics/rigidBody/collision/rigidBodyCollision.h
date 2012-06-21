////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on June 21th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rigid Body collision data

#ifndef _REV_PHYSICS_RIGIDBODY_COLLISION_RIGIDBODYCOLLISION_H_
#define _REV_PHYSICS_RIGIDBODY_COLLISION_RIGIDBODYCOLLISION_H_

#include <revCore/math/vector.h>

namespace rev { namespace physics
{
	class CRigidBody;

	class CRBCollisionInfo
	{
	public:
		CRigidBody * A;
		CRigidBody * B;

		// In coordinates of A
		CVec3	collisionPoint;
		CVec3	collisionNormal;
		float	depth;
	};

}	// namespace physics
}	// namespace rev

#endif // _REV_PHYSICS_RIGIDBODY_COLLISION_RIGIDBODYCOLLISION_H_
