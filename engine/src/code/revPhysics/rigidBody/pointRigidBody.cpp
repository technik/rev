////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on June 21th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Point Rigid Body

#include "pointRigidBody.h"

#include <revPhysics/rigidBody/collision/rigidBodyCollision.h>
#include <revPhysics/rigidBody/sphereRigidBody.h>

namespace rev { namespace physics
{
	//------------------------------------------------------------------------------------------------------------------
	float CPointRigidBody::rayCast(const CVec3& point, const CVec3& unitaryDirection) const
	{
		CVec3 relPos = point - position();
		float sqNorm = relPos.sqNorm();
		float proy = relPos * unitaryDirection;
		if(sqNorm == proy*proy)
		{
			return proy;
		}
		else
			return -1.f;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool CPointRigidBody::checkCollision(CRigidBody * _b, float time, CRBCollisionInfo * _info)
	{
		// Dispatch collitions depending on the type of body we're checking against
		switch(_b->rbType())
		{
		case ePoint:
			{
				return false; // points don't collide each other
			}
		case eSphere:
			{
				return intersectSphere(static_cast<CSphereRigidBody*>(_b), time, _info);
			}
		default:
			// Can't collide unsuported objects
			return false;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	bool CPointRigidBody::intersectSphere(CSphereRigidBody * _b, float _time, CRBCollisionInfo * _info)
	{
		// TODO: This method is unnecesarily slow. Take a look at normalizations for example
		CVec3 relPos = position() - _b->position(); // Position relative to sphere
		CVec3 relVel = linearVelocity() - _b->linearVelocity(); // Velocity relative to sphere
		float r;
		if((r=_b->rayCast(relPos, relVel.normalized())) >= 0.f) // Collision
		{
			float spd = relVel.norm();
			float t = r/spd;
			if(t <= _time)
			{
				_info->A = _b;
				_info->B = this;
				_info->collisionPoint = relPos + (relVel * t);
				_info->collisionNormal = -relVel.normalized();
				_info->depth = (_info->collisionPoint - (relPos+relVel*_time)).norm();
				return true;
			}
			
		}
		return false;
	}

}	// namespace physics
}	// namespace rev
