////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on September 26th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Transform source

#ifndef _REV_REVCORE_TRANSFORM_TRANSFORMSRC_H_
#define _REV_REVCORE_TRANSFORM_TRANSFORMSRC_H_

// Engine headers
#include "revCore/math/matrix.h"
#include "revCore/math/quaternion.h"
#include "revCore/math/vector.h"
#include "rtl/vector.h"

namespace rev
{
	class ITransformSrc
	{
	public:
		const	CVec3&	position	()	const;
		const	CQuat&	rotation	()	const;
		const	CMat34&	transform	()	const;

		ITransformSrc*	parent		()	const;
		void			attachTo	(ITransformSrc* _parent);
	protected:
		void			setPosition	(const CVec3& _position);
		void			setRotation	(const CQuat& _rotation);
		void			setTransform(const CMat34& _transform);

	private:
		typedef rtl::vector<ITransformSrc*>	TChildren;
	private:
		// Methods for internal use
		void			refreshChildren		();
		void			setWorldPos			(const CVec3& _pos);
		void			setLocalPos			(const CVec3& _pos);
		void			setWorldRot			(const CQuat& _rot);
		void			setLocalRot			(const CQuat& _rot);
		// Internal data
		CVec3			mLocalPos;
		CQuat			mLocalRot;
		//CMat34			mLocalTrans;
		CVec3			mWorldPos;
		CQuat			mWorldRot;
		CMat34			mWorldTrans;
		ITransformSrc*	mParent;
		TChildren		mChildren;
	};

	//------------------------------------------------------------------------------------------------------------------
	inline const CVec3& ITransformSrc::position() const
	{
		return mWorldPos;
	}

	//------------------------------------------------------------------------------------------------------------------
	inline const CQuat& ITransformSrc::rotation() const
	{
		return mWorldRot;
	}

	//------------------------------------------------------------------------------------------------------------------
	inline const CMat34& ITransformSrc::transform() const
	{
		return mWorldTrans;
	}

	//------------------------------------------------------------------------------------------------------------------
	inline ITransformSrc * ITransformSrc::parent() const
	{
		return mParent;
	}

	//------------------------------------------------------------------------------------------------------------------
	inline void ITransformSrc::setLocalPos(const CVec3& _pos)
	{
		mLocalPos = _pos;
	}

	//------------------------------------------------------------------------------------------------------------------
	inline void ITransformSrc::setWorldRot(const CQuat& _rot)
	{
		mWorldRot = _rot;
		mWorldTrans = CMat34(_rot, mWorldPos);
	}

	//------------------------------------------------------------------------------------------------------------------
	inline void ITransformSrc::setLocalRot(const CQuat& _rot)
	{
		mLocalRot = _rot;
	}

}	// namespace rev

#endif // _REV_REVCORE_TRANSFORM_TRANSFORMSRC_H_
