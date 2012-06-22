////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on March 15th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Physics world

#ifndef _REV_PHYSICS_WORLD_PHYSICSWORLD_H_
#define _REV_PHYSICS_WORLD_PHYSICSWORLD_H_

#include <vector.h>

namespace rev { namespace physics
{
	// Forward declartions
	class CRigidBody;

	class CPhysicsWorld
	{
	public:
		void	simulate		(float _time);

		void	addRigidBody	(CRigidBody * _body);
		void	removeRigidBody	(CRigidBody * _body);

		unsigned nBodies		() const { return mBodies.size(); }
	private:
		void detectCollisions	(float _interval) const;

	private:
		rtl::vector<CRigidBody*>	mBodies;
	};
}	// namespace physics
}	// namespace rev

#endif // _REV_PHYSICS_WORLD_PHYSICSWORLD_H_