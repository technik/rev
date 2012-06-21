////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on June 15th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sphere Rigid Body

#include "sphereRigidBody.h"

#include <revPhysics/rigidBody/collision/rigidBodyCollision.h>

namespace rev { namespace physics {
	//------------------------------------------------------------------------------------------------------------------
	CSphereRigidBody::CSphereRigidBody(float _radius, float _mass)
		:CRigidBody(_mass)
		,mRadius(_radius)
	{
		setMass(_mass);
	}

	//------------------------------------------------------------------------------------------------------------------
	float CSphereRigidBody::rayCast(const CVec3& _pos, const CVec3& _uDir) const
	{
		// |p+lamda*d - center|^2 <= r^2
		// Transform ray origin to local coordinates of the sphere
		CVec3 relPos = _pos - position();
		// Discriminant
		float proyPos = relPos * _uDir; // Proyection of relative origin over ray distance
		float sqPos = relPos * relPos; // Square magnitude of relative ray origin
		float sqRad = mRadius * mRadius;
		float discriminant = proyPos*proyPos + sqRad - sqPos;
		if(discriminant > 0.f)
		{
			return - proyPos - sqrt(discriminant);
		}
		else
			return -1.f;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CSphereRigidBody::setMass(float _mass)
	{
		mInvMass = _mass==0.f? 0.f : (1.f / _mass);
		mInvInertia = CVec3(mRadius*mRadius*2.5f*mInvMass);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool CSphereRigidBody::checkCollision(CRigidBody * _b, float _time, CRBCollisionInfo * _info)
	{
		switch(_b->rbType())
		{
		case ePoint:
			return _b->checkCollision(this, _time, _info);
		default:
			return false;
		}
	}

}	// namespace physics
}	// namespace rev