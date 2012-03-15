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
	void ITransformSrc::attachTo(ITransformSrc * _parent)
	{
		deattach();
		mParent = _parent;
		if(!mParent)
			return;
		CMat34 parentInvTransform;
		mParent->mWorldTrans.inverse(parentInvTransform);
		mLocalPos = parentInvTransform * mWorldPos;
		mLocalRot = mParent->mWorldRot.inverse().rotate(mWorldRot);
		mParent->mChildren.push_back(this);
	}

	//------------------------------------------------------------------------------------------------------------------
	void ITransformSrc::deattach()
	{
		if(0 != mParent)
		{
			for(rtl::vector<ITransformSrc*>::iterator i = mParent->mChildren.begin(); i != mParent->mChildren.end(); ++i)
			{
				if((*i)==this)
				{
					mParent->mChildren.erase(i);
					break;
				}
			}
			ITransformSrc * parent = mParent;
			mParent = 0;
			CMat34 parentTrans = parent->mWorldTrans;
			setPosition(parent->mWorldTrans * mLocalPos);
			setRotation(parent->mWorldRot.rotate(mLocalRot));
		}
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
		refreshChildren();
	}

	//------------------------------------------------------------------------------------------------------------------
	void ITransformSrc::setRotation(const CQuat& _rotation)
	{
		setWorldRot(_rotation);
		if(mParent)
			setLocalRot(mParent->mWorldRot.inverse() * _rotation);
		refreshChildren();
	}

	//------------------------------------------------------------------------------------------------------------------
	void ITransformSrc::setTransform(const CMat34& _transform)
	{
		mWorldTrans = _transform;
		setPosition(CVec3(_transform.m[0][3], _transform.m[1][3], _transform.m[2][3]));
		setRotation(CQuat(_transform));
		refreshChildren();
	}

	//------------------------------------------------------------------------------------------------------------------
	void ITransformSrc::refreshChildren() const
	{
		for(TChildren::const_iterator i = mChildren.begin(); i != mChildren.end(); ++i)
		{
			(*i)->refreshWorld();
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void ITransformSrc::refreshWorld()
	{
		setWorldPos(mParent->mWorldTrans * mLocalPos);
		setWorldRot(mParent->mWorldRot.rotate(mLocalRot));
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