////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on March 15th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rigid Body

#ifndef _REV_PHYSICS_RIGIDBODY_RIGIDBODY_H_
#define _REV_PHYSICS_RIGIDBODY_RIGIDBODY_H_

#include <revCore/transform/transformSrc.h>

class btCollisionShape;
struct btDefaultMotionState;
class btRigidBody;

namespace rev { namespace physics
{
	class CPhysicsWorld;
	class CRBCollisionInfo;

	class CRigidBody : public ITransformSrc
	{
	public:
		enum ERigidBodyType
		{
			ePoint,
			eSphere,
			eBox,
			eConvexHull,
			eConcaveMesh
		};

	public:
		CRigidBody();
		~CRigidBody();

		// Accessors
		void			setPosition		(const CVec3& _pos);
		void			applyImpulse	(const CVec3& _impulse); // At center of mass

		void			setMass			(float _mass);

	protected:
		btCollisionShape*		mCollisionShape;
		btDefaultMotionState*	mMotionState;
		btRigidBody*			mBulletRigidBody;

	private:
		CPhysicsWorld * mWorld;

		void updateMotionState();
		friend class CPhysicsWorld;
	};

}	// namespace physics
}	// namespace rev

#endif // _REV_PHYSICS_RIGIDBODY_RIGIDBODY_H_