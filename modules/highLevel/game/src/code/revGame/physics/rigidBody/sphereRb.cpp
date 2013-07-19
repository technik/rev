//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On 2013/July/18
//----------------------------------------------------------------------------------------------------------------------
// Physics world

#include "sphereRb.h"

#include <bullet/btBulletDynamicsCommon.h>

namespace rev { namespace game {

	//------------------------------------------------------------------------------------------------------------------
	SphereRb::SphereRb(float _mass, float _radius)
	{
		btCollisionShape* shape = new btSphereShape(_radius);
		btVector3 inertia;
		shape->calculateLocalInertia(_mass, inertia);
		initRb(_mass, inertia, shape);
	}
	
}	// namespace game
}	// namespace rev
