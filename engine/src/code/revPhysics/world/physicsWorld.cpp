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
}	// namespace physics
}	// namespace rev