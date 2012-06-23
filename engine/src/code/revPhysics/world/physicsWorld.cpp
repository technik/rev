////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on March 15th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Physics world

#include "physicsWorld.h"

#include <revPhysics/rigidBody/rigidBody.h>

namespace rev { namespace physics
{
	//------------------------------------------------------------------------------------------------------------------
	void CPhysicsWorld::simulate(float _time)
	{
		// Collision detection
		mCollisions.clear();
		detectCollisions(_time);
		// Constraint preparation
		// Constraint solve
		solveConstraints(_time);
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
	void CPhysicsWorld::detectCollisions(float _interval)
	{
		/// TODO: We're checking collision before integration. We should simulate integration before checking or something
		CRBCollisionInfo info;
		// Iterate over all bodies
		for(rtl::vector<CRigidBody*>::iterator i = mBodies.begin(); i != mBodies.end(); ++i)
		{
			rtl::vector<CRigidBody*>::iterator j = i;
			++j;
			for( ;j != mBodies.end(); ++j)
			{
				// Check collision
				if((*i)->checkCollision(*j, _interval, &info))
				{
					// store collision info;
					mCollisions.push_back(info);
				}
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------------------------
	void CPhysicsWorld::solveConstraints(float _time)
	{
		_time;
		for(rtl::vector<CRBCollisionInfo>::iterator i = mCollisions.begin(); i < mCollisions.end(); ++i)
		{
			//
			// Compute observed masses
			// Move back to collision point.
			// Reverse relative speed at contact point (temporarily just set them 0)
		}
	}

}	// namespace physics
}	// namespace rev