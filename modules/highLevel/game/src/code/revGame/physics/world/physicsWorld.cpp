//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On 2013/July/18
//----------------------------------------------------------------------------------------------------------------------
// Physics world

#include "physicsWorld.h"
#include "../rigidBody/rigidBody.h"

namespace rev { namespace game {

	//------------------------------------------------------------------------------------------------------------------
	PhysicsWorld* PhysicsWorld::sInstance = nullptr;

	//------------------------------------------------------------------------------------------------------------------
	PhysicsWorld* PhysicsWorld::get()
	{
		if(nullptr == sInstance)
			sInstance = new PhysicsWorld;
		return sInstance;
	}

	//------------------------------------------------------------------------------------------------------------------
	void PhysicsWorld::addRigidBody(RigidBody* _rb)
	{
		mBulletWorld->addRigidBody(_rb->mBody);
		mBodies.push_back(_rb);
	}

	//------------------------------------------------------------------------------------------------------------------
	void PhysicsWorld::removeRigidBody(RigidBody* _rb)
	{
		mBulletWorld->removeRigidBody(_rb->mBody);
		for(auto i = mBodies.begin(); i != mBodies.end(); ++i)
		{
			if(*i == _rb) {
				mBodies.erase(i);
				break;
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void PhysicsWorld::update(float _dt)
	{
		mBulletWorld->stepSimulation(_dt);
		for(auto body : mBodies)
			body->refresh();
	}

	//------------------------------------------------------------------------------------------------------------------
	void PhysicsWorld::setGravity(float _g)
	{
		mBulletWorld->setGravity(btVector3(0.f, 0.f, -_g));
	}

	//------------------------------------------------------------------------------------------------------------------
	PhysicsWorld::PhysicsWorld()
	{
		mCollisionConfig = new btDefaultCollisionConfiguration;
		mDispatcher = new btCollisionDispatcher(mCollisionConfig);
		mBroadPhase = new btDbvtBroadphase;
		mSolver = new btSequentialImpulseConstraintSolver;
		mBulletWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadPhase, mSolver, mCollisionConfig);
		mBulletWorld->setGravity(btVector3(0.f,0.f,0.f));
	}

	//------------------------------------------------------------------------------------------------------------------
	PhysicsWorld::~PhysicsWorld()
	{
	}

}	// namespace game
}	// namespace rev
