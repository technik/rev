////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on June 15th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sphere Rigid Body

#include "sphereRigidBody.h"

#include <revPhysics/rigidBody/collision/rigidBodyCollision.h>

#include <libs/bullet/btBulletDynamicsCommon.h>

namespace rev { namespace physics {
	//------------------------------------------------------------------------------------------------------------------
	CSphereRigidBody::CSphereRigidBody(float _radius, float _mass)
		:mRadius(_radius)
	{
		mCollisionShape = new btSphereShape(_radius);

		btTransform transform;
		transform.setIdentity();

		bool isDynamic = (_mass != 0.f);

		btVector3 localInertia(0,0,0);
		if (isDynamic)
			mCollisionShape->calculateLocalInertia(_mass,localInertia);

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		mMotionState = new btDefaultMotionState(transform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(_mass,mMotionState,mCollisionShape,localInertia);
		mBulletRigidBody = new btRigidBody(rbInfo);
	}

}	// namespace physics
}	// namespace rev