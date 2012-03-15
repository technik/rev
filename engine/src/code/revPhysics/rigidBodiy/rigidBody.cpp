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
	CRigidBody::CRigidBody()
		:mLinearSpeed(CVec3::zero)
		,mAngularSpeed(CVec3::zero)
	{
	}

	//-----------------------------------------------------------------------------------------------------------------
	CRigidBody::~CRigidBody()
	{
	}

	//-----------------------------------------------------------------------------------------------------------------
	void CRigidBody::integrate( float _time )
	{
		setPosition(position() + mLinearSpeed * _time);
		if(!(mAngularSpeed == CVec3::zero))
		{
			setRotation(CQuat(mAngularSpeed * _time).rotate(rotation()));
		}
	}

}	// namespace physics
}	// namespace rev