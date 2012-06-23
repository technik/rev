////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on March 15th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Physics world

#ifndef _REV_PHYSICS_WORLD_PHYSICSWORLD_H_
#define _REV_PHYSICS_WORLD_PHYSICSWORLD_H_

#include <revPhysics/rigidBody/collision/rigidBodyCollision.h>
#include <vector.h>

namespace rev { namespace physics
{
	// Forward declartions
	class CRigidBody;

	class CPhysicsWorld
	{
	public:
				CPhysicsWorld	();
		void	simulate		(float _time);

		void	addRigidBody	(CRigidBody * _body);
		void	removeRigidBody	(CRigidBody * _body);

	private:
		void detectCollisions	(float _interval);
		void solveConstraints	(float _interval);

	private:
		rtl::vector<CRigidBody*>		mBodies;
		rtl::vector<CRBCollisionInfo>	mCollisions;
	};
}	// namespace physics
}	// namespace rev

#endif // _REV_PHYSICS_WORLD_PHYSICSWORLD_H_