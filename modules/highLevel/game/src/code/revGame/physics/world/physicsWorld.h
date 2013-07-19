//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On 2013/July/18
//----------------------------------------------------------------------------------------------------------------------
// Physics world

#ifndef _REV_GAME_PHYSICS_WORLD_PHYSICSWORLD_H_
#define _REV_GAME_PHYSICS_WORLD_PHYSICSWORLD_H_

#include <bullet/btBulletDynamicsCommon.h>
#include <vector>

namespace rev { namespace game {

	class RigidBody;

	class PhysicsWorld {
	public:
		static PhysicsWorld* get();

		void	addRigidBody	(RigidBody*);
		void	removeRigidBody	(RigidBody*);

		void	update			(float _dt);
		void	setGravity		(float _g);

	private:
		static PhysicsWorld* sInstance;
		PhysicsWorld();
		~PhysicsWorld();

	private:
		std::vector<RigidBody*>				mBodies;

		btDefaultCollisionConfiguration*	mCollisionConfig;
		btCollisionDispatcher*				mDispatcher;
		btDbvtBroadphase*					mBroadPhase;
		btConstraintSolver*					mSolver;
		btDiscreteDynamicsWorld*			mBulletWorld;
	};
}	// namespace game
}	// namespace rev

#endif // _REV_GAME_PHYSICS_WORLD_PHYSICSWORLD_H_