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
		CRigidBody(float _mass = 0.f);
		~CRigidBody();

		// integrate
		void	integrate	(float _time);

		// Forces, torques and impulses (in global coordinates)
		void	applyForce			(const CVec3& _f, const CVec3& _pos);
		void	applyForce			(const CVec3& _f);
		void	applyTorque			(const CVec3& _t);
		void	applyImpulse		(const CVec3& _i, const CVec3& _pos);
		void	applyImpulse		(const CVec3& _i);
		void	applyTorqueImpulse	(const CVec3& _t);
		void	clearForces			();
		void	clearTorques		();

		// Accessors
				CVec3&	linearVelocity	();
		const	CVec3&	linearVelocity	() const;
				CVec3&	angularVelocity	();
		const	CVec3&	angularVelocity	() const;

		virtual void	setMass			(float _mass);
		void			setPosition		(const CVec3& _pos);
		void			setRotation		(const CQuat& _rot);

		virtual ERigidBodyType	rbType	() const = 0;

		// Ray casting
		// Returns distance if the ray intersects this body, -1.f else
		virtual float	rayCast(const CVec3& point, const CVec3& direction) const = 0;
		// Returns true in case of collision and fills rigidBodyCollisionStructure
		virtual	bool	checkCollision	(CRigidBody * B, float time, CRBCollisionInfo * info) = 0;

	protected:
		float	mInvMass;
		CVec3	mInvInertia;

	private:
		CVec3	mLinearVelocity;
		CVec3	mAngularVelocity;

		CVec3	mForce;	// Forces applied at the center of mass
		CVec3	mTorque;	// Applied torques

		CPhysicsWorld * mWorld;
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