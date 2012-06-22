////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on March 15th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Physics world

#include "physicsWorld.h"

#include <revPhysics/rigidBody/collision/rigidBodyCollision.h>
#include <revPhysics/rigidBody/rigidBody.h>

namespace rev { namespace physics
{
	//------------------------------------------------------------------------------------------------------------------
	void CPhysicsWorld::simulate(float _time)
	{
		// Collision detection
		detectCollisions(_time);
		// Constraint preparation
		// Constraint solve
		// Integration
		for(rtl::vector<CRigidBody*>::iterator i = mBodies.begin(); i != mBodies.end(); ++i)
		{
			CRigidBody * body = *i;
			body->integrate(_time);
			body->clearForces();
			body->clearTorques();
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CPhysicsWorld::addRigidBody(CRigidBody * _body)
	{
		mBodies.push_back(_body);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CPhysicsWorld::removeRigidBody(CRigidBody * _body)
	{
		for(rtl::vector<CRigidBody*>::iterator i = mBodies.begin(); i != mBodies.end(); ++i)
		{
			if(*i == _body)
			{
				mBodies.erase(i);
				return;
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CPhysicsWorld::detectCollisions(float _interval) const
	{
		CRBCollisionInfo info;
		// Iterate over all bodies
		for(rtl::vector<CRigidBody*>::const_iterator i = mBodies.begin(); i != mBodies.end(); ++i)
		{
			rtl::vector<CRigidBody*>::const_iterator j = i;
			++j;
			for( ;j != mBodies.end(); ++j)
			{
				// Check collision
				if((*i)->checkCollision(*j, _interval, &info))
				{
					// Stop colliding bodies;
					(*i)->linearVelocity() = CVec3::zero;
					(*j)->linearVelocity() = CVec3::zero;
				}
			}
		}
	}
}	// namespace physics
}	// namespace rev