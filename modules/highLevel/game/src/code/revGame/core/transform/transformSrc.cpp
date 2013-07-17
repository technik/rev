//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On 2013/July/12
//----------------------------------------------------------------------------------------------------------------------
// Generic transform source

#include "transformSrc.h"

using namespace rev::math;

namespace rev { namespace game {

	//------------------------------------------------------------------------------------------------------------------
	TransformSrc::~TransformSrc()
	{
		deattach();
	}

	//------------------------------------------------------------------------------------------------------------------
	void TransformSrc::attachTo(TransformSrc* _parent)
	{
		deattach();
		if(nullptr != _parent)
		{
			// Stablish hierarchy link
			mParent = _parent;
			mParent->mChildren.push_back(this);

			refreshLocal();// Compute new local coordinates
			// Note: world coordinates remain the same along the attach process. So do children
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void TransformSrc::releaseChildren()
	{
		for(auto child : mChildren)
			child->mParent = nullptr;
		mChildren.clear();
	}

	//------------------------------------------------------------------------------------------------------------------
	TransformSrc::TransformSrc()
		:mLocalPos(Vec3f::zero())
		,mLocalRot(Quatf::identity())
		,mLocalTrans(Mat34f::identity())
		,mWorldPos(Vec3f::zero())
		,mWorldRot(Quatf::identity())
		,mWorldTrans(Mat34f::identity())
		,mParent(nullptr)
	{}

	//------------------------------------------------------------------------------------------------------------------
	void TransformSrc::setLocalPos(const Vec3f& _pos, bool _refreshChildren)
	{
		mLocalPos = _pos;
		mLocalTrans = Mat34f(mLocalRot, mLocalPos);
		refreshWorld();
		if(_refreshChildren) refreshChildren();
	}

	//------------------------------------------------------------------------------------------------------------------
	void TransformSrc::setLocalRot(const Quatf& _rot, bool _refreshChildren)
	{
		mLocalRot = _rot;
		mLocalTrans = Mat34f(mLocalRot, mLocalPos);
		refreshWorld();
		if(_refreshChildren) refreshChildren();
	}

	//------------------------------------------------------------------------------------------------------------------
	void TransformSrc::setLocalTrans(const Mat34f& _trans, bool _refreshChildren)
	{
		mLocalTrans = _trans;
		mLocalRot = Quatf(mLocalTrans);
		mLocalPos = mLocalTrans * Vec3f::zero();
		refreshWorld();
		if(_refreshChildren) refreshChildren();
	}

	//------------------------------------------------------------------------------------------------------------------
	void TransformSrc::setWorldPos(const Vec3f& _pos, bool _refreshChildren)
	{
		mWorldPos = _pos;
		mWorldTrans = Mat34f(mWorldRot, mWorldPos);
		refreshLocal();
		if(_refreshChildren) refreshChildren();
	}

	//------------------------------------------------------------------------------------------------------------------
	void TransformSrc::setWorldRot(const Quatf& _rot, bool _refreshChildren)
	{
		mWorldRot = _rot;
		mWorldTrans = Mat34f(mWorldRot, mWorldPos);
		refreshLocal();
		if(_refreshChildren) refreshChildren();
	}

	//------------------------------------------------------------------------------------------------------------------
	void TransformSrc::setWorldTrans(const Mat34f& _trans, bool _refreshChildren)
	{
		mWorldTrans = _trans;
		mWorldRot = Quatf(mWorldTrans);
		mWorldPos = mWorldTrans * Vec3f::zero();
		refreshLocal();
		if(_refreshChildren) refreshChildren();
	}

	//------------------------------------------------------------------------------------------------------------------
	void TransformSrc::deattach()
	{
		if(nullptr != mParent)
		{
			// Clean hierarchy
			auto& siblings = mParent->mChildren;
			for(auto i = siblings.begin(); i != siblings.end(); ++i)
			{
				if(*i == this) {
					i = siblings.erase(i);
					break;
				}
			}
			// Update local coordinates
			mLocalPos = mWorldPos; 
			mLocalRot = mWorldRot;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void TransformSrc::refreshChildren()
	{
		for(auto child : mChildren)
			child->refreshWorld();
	}

	//------------------------------------------------------------------------------------------------------------------
	void TransformSrc::refreshLocal()
	{
		Mat34f parentInv;
		if(mParent != nullptr) {
			mParent->mWorldTrans.inverse(parentInv);
			mLocalPos = parentInv * mWorldPos;
			mLocalRot = mParent->rotation().inverse() * mLocalRot;
			mLocalTrans = Mat34f(mLocalRot, mLocalPos);
		} else {
			mLocalPos = mWorldPos;
			mLocalRot = mWorldRot;
			mLocalTrans = mWorldTrans;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void TransformSrc::refreshWorld()
	{
		if(mParent != nullptr) {
			mWorldPos = mParent->transform() * mLocalPos;
			mWorldRot = mParent->rotation() * mLocalRot;
			mWorldTrans = Mat34f(mWorldRot, mWorldPos);
		} else {
			mWorldPos = mLocalPos;
			mWorldRot = mLocalRot;
			mWorldTrans = mLocalTrans;
		}
	}

}	// namespace game
}	// namespace rev