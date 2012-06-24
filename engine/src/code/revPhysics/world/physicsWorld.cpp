////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on March 15th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Physics world

#include "physicsWorld.h"

#include <revPhysics/rigidBody/rigidBody.h>

#include <libs/bullet/btBulletDynamicsCommon.h>

namespace rev { namespace physics
{
	//------------------------------------------------------------------------------------------------------------------
	CPhysicsWorld::CPhysicsWorld()
	{
		// Create collision configuration object
		mCollisionConfig = new btDefaultCollisionConfiguration();
		mDispatcher = new	btCollisionDispatcher(mCollisionConfig);
		mBroadPhase = new btDbvtBroadphase();
		mSolver = new btSequentialImpulseConstraintSolver();
		mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher,mBroadPhase,mSolver,mCollisionConfig);
		mDynamicsWorld->setGravity(btVector3(0, 0,0));
	}
	
	//------------------------------------------------------------------------------------------------------------------
	CPhysicsWorld::~CPhysicsWorld()
	{
		// delete bullet data
		delete mDynamicsWorld;
		delete mSolver;
		delete mBroadPhase;
		delete mDispatcher;
		delete mCollisionConfig;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CPhysicsWorld::simulate(float _time)
	{
		mDynamicsWorld->stepSimulation(_time);
		// Update rigid body transforms
		for(unsigned i = 0; i < mBodies.size(); ++i)
		{
			mBodies[i]->updateMotionState();
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CPhysicsWorld::addRigidBody(CRigidBody * _body)
	{
		mBodies.push_back(_body);
		mDynamicsWorld->addRigidBody(_body->mBulletRigidBody);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CPhysicsWorld::removeRigidBody(CRigidBody * _body)
	{
		for(rtl::vector<CRigidBody*>::iterator i = mBodies.begin(); i != mBodies.end(); ++i)
		{
			if(*i == _body)
			{
				mDynamicsWorld->removeRigidBody(_body->mBulletRigidBody);
				mBodies.erase(i);
				return;
			}
		}
	}

}	// namespace physics
}	// namespace rev