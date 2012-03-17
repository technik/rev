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
		CRigidBody(float _mass = 0.f);
		~CRigidBody();

		// integrate
		void	integrate	(float _time);

		// Forces, torques and impulses (in global coordinates)
		void	applyForce			(const CVec3& _f, const CVec3& _pos);
		void	applyTorque			(const CVec3& _t);
		void	applyImpulse		(const CVec3& _i, const CVec3& _pos);
		void	applyTorqueImpulse	(const CVec3& _t);
		void	clearForces			();
		void	clearTorques		();

		// Accessors
				CVec3&	linearVelocity	();
		const	CVec3&	linearVelocity	() const;
				CVec3&	angularVelocity	();
		const	CVec3&	angularVelocity	() const;

		void			setMass			(float _mass);
		void			setPosition		(const CVec3& _pos);
		void			setRotation		(const CQuat& _rot);

	private:
		CVec3	mLinearVelocity;
		CVec3	mAngularVelocity;

		float	mInvMass;
		CVec3	mInertia;

		CVec3	mForce;	// Forces applied at the center of mass
		CVec3	mTorque;	// Applied torques

		static CPhysicsWorld * defaultWorld;
	};

	// Inline implementations
	inline			CVec3&	CRigidBody::linearVelocity	()			{ return mLinearVelocity;	}
	inline	const	CVec3&	CRigidBody::linearVelocity	() const	{ return mLinearVelocity;	}
	inline			CVec3&	CRigidBody::angularVelocity	()			{ return mAngularVelocity;	}
	inline	const	CVec3&	CRigidBody::angularVelocity	() const	{ return mAngularVelocity;	}
	inline	void			CRigidBody::clearForces		()			{ mForce = CVec3::zero;		}
	inline	void			CRigidBody::clearTorques	()			{ mTorque = CVec3::zero;	}
	inline	void			CRigidBody::setPosition		(const CVec3& _pos)	{	ITransformSrc::setPosition(_pos);	}
	inline	void			CRigidBody::setRotation		(const CQuat& _rot)	{	ITransformSrc::setRotation(_rot);	}

}	// namespace physics
}	// namespace rev

#endif // _REV_PHYSICS_RIGIDBODY_RIGIDBODY_H_