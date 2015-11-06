//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Physics simulation context
#include "physicsWorld.h"
#include "rigidBody.h"

namespace rev {
	namespace game {
		//--------------------------------------------------------------------------------------------------------------
		PhysicsWorld::PhysicsWorld() {
			mCollisionConfig = new btDefaultCollisionConfiguration();
			mDispatcher = new btCollisionDispatcher(mCollisionConfig);
			mOverlappingPairCache = new btDbvtBroadphase();
			mSolver = new btSequentialImpulseConstraintSolver();
			mWorld = new btDiscreteDynamicsWorld(mDispatcher,
				mOverlappingPairCache, mSolver, mCollisionConfig);
			mWorld->setGravity(btVector3(0.f, 0.f, -9.819f));
		}

		//--------------------------------------------------------------------------------------------------------------
		PhysicsWorld::~PhysicsWorld() {
			delete mWorld;
			delete mSolver;
			delete mOverlappingPairCache;
			delete mDispatcher;
			delete mCollisionConfig;
		}

		//--------------------------------------------------------------------------------------------------------------
		void PhysicsWorld::addRigidBody(RigidBody* _rb) {
			mWorld->addRigidBody(_rb->mBody);
			mBodies.insert(_rb);
		}

		//--------------------------------------------------------------------------------------------------------------
		void PhysicsWorld::removeRigidBody(RigidBody* _rb) {
			mWorld->removeRigidBody(_rb->mBody);
			mBodies.erase(_rb);
		}

		//--------------------------------------------------------------------------------------------------------------
		void PhysicsWorld::simulate(float _dt) {
			float fixedTimeStep = 0.01f;
			unsigned maxSteps = unsigned(1.5f*_dt / fixedTimeStep);

			mWorld->stepSimulation(_dt, maxSteps, fixedTimeStep);
		}
	}
}