////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on March 15th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rigid Body

#include "rigidBody.h"

#include <revCore/codeTools/assert/assert.h>

namespace rev { namespace physics
{
	// Static data definitin
	CPhysicsWorld * CRigidBody::defaultWorld = 0;

	//-----------------------------------------------------------------------------------------------------------------
	CRigidBody::CRigidBody(float _mass)
		:mInvMass(0.f)
		,mLinearVelocity(CVec3::zero)
		,mAngularVelocity(CVec3::zero)
		,mForce(CVec3::zero)
		,mTorque(CVec3::zero)
	{
		setMass(_mass);
	}

	//-----------------------------------------------------------------------------------------------------------------
	CRigidBody::~CRigidBody()
	{
	}

	//-----------------------------------------------------------------------------------------------------------------
	void CRigidBody::integrate( float _time )
	{
		// Apply forces
		applyImpulse(mForce * _time);
		applyTorqueImpulse(mTorque * _time);
		// Apply friction
		applyFriction();
		// Integrate motion
		setPosition(position() + mLinearVelocity * _time);
		if(!(mAngularVelocity == CVec3::zero))
		{
			setRotation(CQuat(mAngularVelocity * _time) * rotation());
		}
	}

	//-----------------------------------------------------------------------------------------------------------------
	void CRigidBody::applyForce(const CVec3& _f, const CVec3& _pos)
	{
		mForce += _f;
		CVec3 distanceToG = _pos - position();
		mTorque += _f ^ distanceToG;
	}

	//-----------------------------------------------------------------------------------------------------------------
	void CRigidBody::applyForce(const CVec3& _f)
	{
		mForce += _f;
	}

	//-----------------------------------------------------------------------------------------------------------------
	void CRigidBody::applyTorque(const CVec3& _t)
	{
		mTorque += _t;
	}

	//-----------------------------------------------------------------------------------------------------------------
	void CRigidBody::applyImpulse(const CVec3& _i, const CVec3& _pos)
	{
		mLinearVelocity += _i*mInvMass;
		CVec3 distanceToG = _pos - position();
		mAngularVelocity +=(_i ^ distanceToG) | mInvInertia;
	}

	//-----------------------------------------------------------------------------------------------------------------
	void CRigidBody::applyImpulse(const CVec3& _i)
	{
		mLinearVelocity += _i*mInvMass;
	}

	//-----------------------------------------------------------------------------------------------------------------
	void CRigidBody::applyTorqueImpulse(const CVec3& _t)
	{
		mAngularVelocity += _t | mInvInertia;
	}

	//-----------------------------------------------------------------------------------------------------------------
	void CRigidBody::setMass(float _mass)
	{
		mInvMass = _mass==0.f? 0.f : (1.f / _mass);
		mInvInertia = CVec3(5.f*mInvMass/2.f); // Assuming unitary radius sphere
	}

	//-----------------------------------------------------------------------------------------------------------------
	void CRigidBody::applyFriction()
	{
		// TODO
	}

}	// namespace physics
}	// namespace rev