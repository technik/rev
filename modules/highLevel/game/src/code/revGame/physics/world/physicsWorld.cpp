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
	}

	//------------------------------------------------------------------------------------------------------------------
	void PhysicsWorld::removeRigidBody(RigidBody*)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	void PhysicsWorld::update(float _dt)
	{
		mBulletWorld->stepSimulation(_dt);
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
