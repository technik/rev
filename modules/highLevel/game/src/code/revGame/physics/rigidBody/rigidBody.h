//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On 2013/July/18
//----------------------------------------------------------------------------------------------------------------------
// Physics rigid body

#ifndef _REV_GAME_PHYSICS_RIGIDBODY_RIGIDBODY_H_
#define _REV_GAME_PHYSICS_RIGIDBODY_RIGIDBODY_H_

#include <bullet/btBulletDynamicsCommon.h>
#include <revMath/algebra/vector.h>
#include <revGame/core/transform/transformSrc.h>

namespace rev { namespace game {

	class RigidBody : public TransformSrc {
	public:
		virtual ~RigidBody();
		void setPosition	(const math::Vec3f& _position);

		void applyImpulse	(const math::Vec3f& _impulse);
		// Position is given in world coordinates, relative to the rigid body's COM
		void applyImpulse	(const math::Vec3f& _impulse, const math::Vec3f& _position);

		void refresh		();

	public:
		static RigidBody* sphere	(float _mass, float _radius);
		static RigidBody* box		(float _mass, const math::Vec3f& _size);

	private:
		RigidBody(float _mass, const btVector3& _inertia, btCollisionShape* _shape);

	private:
		btMotionState*	mMotionState;
		btRigidBody*	mBody;

		friend class PhysicsWorld;
	};
}	// namespace game
}	// namespace rev

#endif // _REV_GAME_PHYSICS_RIGIDBODY_RIGIDBODY_H_