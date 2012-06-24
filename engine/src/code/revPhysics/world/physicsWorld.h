////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on March 15th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Physics world

#ifndef _REV_PHYSICS_WORLD_PHYSICSWORLD_H_
#define _REV_PHYSICS_WORLD_PHYSICSWORLD_H_

#include <vector.h>

class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
struct btDbvtBroadphase;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;

namespace rev { namespace physics
{
	// Forward declartions
	class CRigidBody;

	class CPhysicsWorld
	{
	public:
				CPhysicsWorld	();
				~CPhysicsWorld	();
		void	simulate		(float _time);

		void	addRigidBody	(CRigidBody * _body);
		void	removeRigidBody	(CRigidBody * _body);

	private:

	private:
		rtl::vector<CRigidBody*>		mBodies;

		// Bullet integration data
		btDefaultCollisionConfiguration*		mCollisionConfig;
		btCollisionDispatcher*					mDispatcher;
		btDbvtBroadphase*						mBroadPhase;
		btSequentialImpulseConstraintSolver*	mSolver;
		btDiscreteDynamicsWorld*				mDynamicsWorld;
	};
}	// namespace physics
}	// namespace rev

#endif // _REV_PHYSICS_WORLD_PHYSICSWORLD_H_