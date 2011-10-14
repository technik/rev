////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on September 26th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Transform source

#include "transformSrc.h"

//----------------------------------------------------------------------------------------------------------------------
namespace rev
{
	//------------------------------------------------------------------------------------------------------------------
	inline void ITransformSrc::attachTo(ITransformSrc * _parent)
	{
		mParent = _parent;
		if(!mParent)
			return;
		CMat34 parentInvTransform;
		mParent->mWorldTrans.inverse(parentInvTransform);
		mLocalPos = parentInvTransform * mWorldPos;
		mLocalRot = mParent->mWorldRot * mWorldRot;
	}

	//------------------------------------------------------------------------------------------------------------------
	void ITransformSrc::setPosition(const CVec3& _pos)
	{
		setWorldPos(_pos);
		if(mParent)
		{
			CMat34 parentInvTransform;
			mParent->mWorldTrans.inverse(parentInvTransform);
			setLocalPos(parentInvTransform * _pos);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void ITransformSrc::setRotation(const CQuat& _rotation)
	{
		setWorldRot(_rotation);
		if(mParent)
			setLocalRot(mParent->mWorldRot.inverse() * _rotation);
	}

	//------------------------------------------------------------------------------------------------------------------
	void ITransformSrc::setTransform(const CMat34& _transform)
	{
		mWorldTrans = _transform;
		setPosition(CVec3(_transform.m[0][3], _transform.m[1][3], _transform.m[2][3]));
		setRotation(CQuat(_transform));
	}

	//------------------------------------------------------------------------------------------------------------------
	void ITransformSrc::refreshChildren()
	{
		for(TChildren::iterator i = mChildren.begin(); i != mChildren.end(); ++i)
		{
			ITransformSrc * child = *i;
			child->setPosition(child->mWorldPos);
			child->setRotation(child->mWorldRot);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void ITransformSrc::setWorldPos(const CVec3& _pos)
	{
		mWorldPos = _pos;
		mWorldTrans.m[0][3] = _pos.x;
		mWorldTrans.m[1][3] = _pos.y;
		mWorldTrans.m[2][3] = _pos.z;
	}

}	// namespace rev