//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Physics simulation context
#ifndef _REV_GAME_PHYSICS_PHYSICSWORLD_H_
#define _REV_GAME_PHYSICS_PHYSICSWORLD_H_

#include <btBulletDynamicsCommon.h>

namespace rev {
	namespace game {
		class PhysicsWorld {
		private:
			btDefaultCollisionConfiguration*	mCollisionConfig;
			btCollisionDispatcher*				mDispatcher;
			btBroadphaseInterface*				mOverlappingPairCache;
			btSequentialImpulseConstraintSolver*mSolver;
			btDiscreteDynamicsWorld*			mWorld;
		};
	}
}

#endif // _REV_GAME_PHYSICS_PHYSICSWORLD_H_
