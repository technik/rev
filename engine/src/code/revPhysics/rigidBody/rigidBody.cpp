////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on March 15th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rigid Body

#include "rigidBody.h"
#include <revPhysics/world/physicsWorld.h>

#include <revCore/codeTools/assert/assert.h>

#include <libs/bullet/btBulletDynamicsCommon.h>

namespace rev { namespace physics
{
	//-----------------------------------------------------------------------------------------------------------------
	CRigidBody::CRigidBody()
		:mWorld(0)
	{
	}

	//-----------------------------------------------------------------------------------------------------------------
	CRigidBody::~CRigidBody()
	{
		delete mCollisionShape;
		if(0 != mWorld)
		{
			mWorld->removeRigidBody(this);
		}
		delete mMotionState;
		delete mBulletRigidBody;
	}

	//-----------------------------------------------------------------------------------------------------------------
	void CRigidBody::setPosition(const CVec3& _pos)
	{
		// Transform position
		ITransformSrc::setPosition(_pos);
		// Bullet position
		btTransform transform;
		mMotionState->getWorldTransform(transform);
		transform.setOrigin(btVector3(_pos.x,_pos.y,_pos.z));
		mMotionState->setWorldTransform(transform);
		mBulletRigidBody->setMotionState(mMotionState);
	}

	//-----------------------------------------------------------------------------------------------------------------
	void CRigidBody::applyImpulse(const CVec3& _i)
	{
		mBulletRigidBody->applyCentralImpulse(btVector3(_i.x,_i.y,_i.z));
	}

	//-----------------------------------------------------------------------------------------------------------------
	void CRigidBody::setMass(float _mass)
	{
		// Compute local inertia
		btVector3 inertia;
		mCollisionShape->calculateLocalInertia(_mass, inertia);
		mBulletRigidBody->setMassProps(_mass, inertia);
	}

	//-----------------------------------------------------------------------------------------------------------------
	void CRigidBody::updateMotionState()
	{
		// Get motion state transform
		btTransform transform;
		mMotionState->getWorldTransform(transform);
		// Update position
		btVector3 pos = transform.getOrigin();
		ITransformSrc::setPosition(CVec3(pos.x(),pos.y(),pos.z()));
		// Update rotation
		btQuaternion rot = transform.getRotation();
		ITransformSrc::setRotation(CQuat(rot.x(),rot.y(),rot.z(),rot.w()));
	}

}	// namespace physics
}	// namespace rev