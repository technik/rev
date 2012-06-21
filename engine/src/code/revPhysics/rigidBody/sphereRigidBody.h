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

		ERigidBodyType	rbType	() const { return eSphere; }

		float	rayCast(const CVec3& point, const CVec3& unitaryDirection) const;
		bool	checkCollision	(CRigidBody * B, float time, CRBCollisionInfo * info);

		void setMass(float _mass);

	private:
		bool intersectParticle(CSphereRigidBody * _b, float _time, CRBCollisionInfo * _info);

	private:
		float	mRadius;
	};
}	// namespace physics
}	// namespace rev

#endif // _REV_PHYSICS_RIGIDBODY_SPHERERIGIDBODY_H_