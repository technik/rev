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

			// Compute new local coordinates
			Mat34f parentInv;
			mParent->transform().inverse(parentInv); // Extract parent's inverse transform to transform local position
			mLocalPos = parentInv * mLocalPos;
			mLocalRot = mParent->rotation().inverse() * mLocalRot;
			refreshCacheCoords();
			// Note: world coordinates remain the same along the attach process
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
		:mLocalPos(Vec3f::identity())
		,mLocalRot(Quatf::identity())
		,mWorldTrans(Mat34f::identity())
		,mWorldPos(Vec3f::identity())
		,mWorldRot(Quatf::identity())
		,mParent(nullptr)
	{}

	//------------------------------------------------------------------------------------------------------------------
	void TransformSrc::setLocalPos(const Vec3f& _pos, bool _refreshChildren)
	{
		mLocalPos = _pos;
		mWorldPos = mParent?(mParent->transform()*_pos):_pos;
		if(_refreshChildren) refreshChildren();
	}

	//------------------------------------------------------------------------------------------------------------------
	void TransformSrc::setLocalRot(const Quatf& _rot, bool _refreshChildren)
	{
		mLocalRot = _rot;
		mWorldRot = mParent?(mParent->rotation()*_rot):_rot;
		if(_refreshChildren) refreshChildren();
	}

	//------------------------------------------------------------------------------------------------------------------
	void TransformSrc::setLocalTrans(const Mat34f& _trans, bool _refreshChildren)
	{
		mWorldRot = mParent?(mParent->transform()*_trans):_trans;
		if(_refreshChildren) refreshChildren();
	}

	//------------------------------------------------------------------------------------------------------------------
	void TransformSrc::deattach()
	{
		if(nullptr != mParent)
		{
			// Clean hierarchy
			auto& siblings = mParent->mChildren;
			for(auto i = siblings.begin(); i != siblings.end; ++i)
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
		{
			child->mWorldPos = mWorldTrans * child->mLocalPos;
			child->mWorldRot = mWorldRot * child->mLocalRot;
			child->mWorldTrans = Mat34f(child->mLocalRot, child->mLocalPos);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void TransformSrc::refreshCacheCoords()
	{
		mWorldPos = mParent->transform() * mLocalPos;
		mWorldRot = mParent->rotation() * mLocalRot;
		mWorldTrans = Mat34f(mWorldRot, mWorldPos);
	}

}	// namespace game
}	// namespace rev