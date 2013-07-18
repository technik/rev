//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On 2013/July/18
//----------------------------------------------------------------------------------------------------------------------
// Physics world

#include "physicsWorld.h"

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
	void PhysicsWorld::addRigidBody(RigidBody*)
	{
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
	}

	//------------------------------------------------------------------------------------------------------------------
	PhysicsWorld::~PhysicsWorld()
	{
	}

}	// namespace game
}	// namespace rev
