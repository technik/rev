////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on June 18th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Box Rigid Body

#ifndef _REV_PHYSICS_RIGIDBODY_BOXRIGIDBODY_H_
#define _REV_PHYSICS_RIGIDBODY_BOXRIGIDBODY_H_

#include "rigidBody.h"
#include <revCore/math/vector.h>

namespace rev { namespace physics
{
	class CBoxRigidBody : public CRigidBody
	{
	public:
		CBoxRigidBody(const CVec3& _size, float _mass = 0.f);

		ERigidBodyType	rbType	() const { return eBox; }

		float	rayCast(const CVec3& point, const CVec3& unitaryDirection) const;
		bool	checkCollision	(CRigidBody * B, float time, CRBCollisionInfo * info);

	private:
		CVec3	mHalfSize;
	};
}	// namespace physics
}	// namespace rev

#endif // _REV_PHYSICS_RIGIDBODY_BOXRIGIDBODY_H_