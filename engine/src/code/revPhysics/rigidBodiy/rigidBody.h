////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on March 15th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rigid Body

#ifndef _REV_PHYSICS_RIGIDBODY_RIGIDBODY_H_
#define _REV_PHYSICS_RIGIDBODY_RIGIDBODY_H_

#include <revCore/transform/transformSrc.h>

namespace rev { namespace physics
{
	class CPhysicsWorld;

	class CRigidBody : public ITransformSrc
	{
	public:
		CRigidBody();
		~CRigidBody();

		// integrate
		void	integrate	(float _time);

		// Accessors
				CVec3&	linearSpeed		();
		const	CVec3&	linearSpeed		() const;
				CVec3&	angularSpeed	();
		const	CVec3&	angularSpeed	() const;

	private:
		CVec3	mLinearSpeed;
		CVec3	mAngularSpeed;

		static CPhysicsWorld * defaultWorld;
	};

	// Inline implementations
	inline			CVec3& CRigidBody::linearSpeed	()			{ return mLinearSpeed;	}
	inline	const	CVec3& CRigidBody::linearSpeed	() const	{ return mLinearSpeed;	}
	inline			CVec3& CRigidBody::angularSpeed	()			{ return mAngularSpeed;	}
	inline	const	CVec3& CRigidBody::angularSpeed	() const	{ return mAngularSpeed; }
}	// namespace physics
}	// namespace rev

#endif // _REV_PHYSICS_RIGIDBODY_RIGIDBODY_H_