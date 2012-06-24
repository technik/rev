////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on June 21th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Point Rigid Body

#ifndef _REV_PHYSICS_RIGIDBODY_POINTRIGIDBODY_H_
#define _REV_PHYSICS_RIGIDBODY_POINTRIGIDBODY_H_

#include "rigidBody.h"

namespace rev { namespace physics
{
	class CSphereRigidBody;

	class CPointRigidBody : public CRigidBody
	{
	public:
		CPointRigidBody(float _mass = 0.f);
	};
}	// namespace physics
}	// namespace rev

#endif // _REV_PHYSICS_RIGIDBODY_POINTRIGIDBODY_H_