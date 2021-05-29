//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Physics simulation context
#ifndef _REV_GAME_PHYSICS_PHYSICSWORLD_H_
#define _REV_GAME_PHYSICS_PHYSICSWORLD_H_

/*#include <btBulletDynamicsCommon.h>
#include <set>
#include <math/algebra/vector.h>

namespace rev {
	namespace game {

		class RigidBody;

		class PhysicsWorld {
		public:
			PhysicsWorld();
			~PhysicsWorld();

			void addRigidBody(RigidBody* _rb);
			void removeRigidBody(RigidBody* _rb);

			void setGravity(const math::Vec3f& _g);

			// Two ways of simulating
			void simulate(float _dt);
			void singleStep(float _dt);

		private:

			btDefaultCollisionConfiguration*	mCollisionConfig;
			btCollisionDispatcher*				mDispatcher;
			btBroadphaseInterface*				mOverlappingPairCache;
			btSequentialImpulseConstraintSolver*mSolver;
			btDiscreteDynamicsWorld*			mWorld;

			std::set<RigidBody*>				mBodies;
		};
	}
}*/

#endif // _REV_GAME_PHYSICS_PHYSICSWORLD_H_
