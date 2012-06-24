////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on June 15th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sphere Rigid Body

#ifndef _REV_PHYSICS_RIGIDBODY_SPHERERIGIDBODY_H_
#define _REV_PHYSICS_RIGIDBODY_SPHERERIGIDBODY_H_

#include "rigidBody.h"

namespace rev { namespace physics
{
	class CSphereRigidBody : public CRigidBody
	{
	public:
		CSphereRigidBody(float _radius, float _mass = 0.f);

	private:
		float	mRadius;
	};
}	// namespace physics
}	// namespace rev

#endif // _REV_PHYSICS_RIGIDBODY_SPHERERIGIDBODY_H_