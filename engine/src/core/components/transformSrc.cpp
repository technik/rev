//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Transform source for the component system
#include "transformSrc.h"

using namespace rev::math;
using std::vector;

namespace rev {
	namespace core {
		//--------------------------------------------------------------------------------------------------------------
		void TransformSrc::attachTo(TransformSrc* _parent) {
			dettach();
			mParent = _parent;
			if (!mParent)
				return;
			Mat34f parentInvTransform;
			mParent->mWorldTrans.inverse(parentInvTransform);
			mLocalPos = parentInvTransform * mWorldPos;
			mLocalRot = mParent->mWorldRot.inverse() * mWorldRot;
			mParent->mChildren.push_back(this);
		}

		//--------------------------------------------------------------------------------------------------------------
		void TransformSrc::dettach() {
			if (mParent)
			{
				for (auto i = mParent->mChildren.begin(); i != mParent->mChildren.end(); ++i)
				{
					if ((*i) == this)
					{
						mParent->mChildren.erase(i);
						break;
					}
				}
				TransformSrc * oldParent = mParent;
				mParent = nullptr;
				Mat34f parentTrans = oldParent->mWorldTrans;
				setPosition(oldParent->mWorldTrans * mLocalPos, global);
				setRotation(oldParent->mWorldRot * mLocalRot, global);
			}
		}
	}	// namespace core
}	// namespace rev