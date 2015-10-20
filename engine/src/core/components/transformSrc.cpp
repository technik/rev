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

		//--------------------------------------------------------------------------------------------------------------
		void TransformSrc::setPosition(const Vec3f& _pos, TransformSrc::CoordinateSystem _base) {
			if(mParent) {
				if (_base == local) {
					setWorldPos(_pos);
					Mat34f parentInvTransform;
					mParent->mWorldTrans.inverse(parentInvTransform);
					setLocalPos(parentInvTransform * _pos);
				}
				else {
					setLocalPos(_pos);
					setWorldPos(mParent->mWorldTrans * _pos);
				}
			}
			else {
				setWorldPos(_pos);
				setLocalPos(_pos);
			}
			refreshChildren();
		}

		//--------------------------------------------------------------------------------------------------------------
		void TransformSrc::setRotation(const Quatf& _rot, TransformSrc::CoordinateSystem _base) {
			if (mParent) {
				if (_base == local) {
					setWorldRot(_rot);
					setLocalRot(mParent->mWorldRot.inverse() * _rot);
				}
				else {
					setLocalRot(_rot);
					setWorldRot(mParent->mWorldRot * _rot);
				}
			}
			else {
				setWorldRot(_rot);
				setLocalRot(_rot);
			}
			refreshChildren();
		}

		//--------------------------------------------------------------------------------------------------------------
		void TransformSrc::setTransform(const Mat34f& _trans, TransformSrc::CoordinateSystem _base) {
			mWorldTrans = _trans;
			Vec3f pos(_trans[0][3], _trans[1][3], _trans[2][3]);
			Quatf rot(_trans);
			setRotation(rot, _base);
			setPosition(pos, _base);
		}

		//--------------------------------------------------------------------------------------------------------------
		void TransformSrc::refreshChildren() const {
			for(auto child : mChildren)
				child->refreshWorld();
		}

		//--------------------------------------------------------------------------------------------------------------
		void TransformSrc::refreshWorld() {
			setWorldPos(mParent->mWorldTrans * mLocalPos);
			setWorldRot(mParent->mWorldRot * mLocalRot);
		}

		//--------------------------------------------------------------------------------------------------------------
		void TransformSrc::setWorldPos(const Vec3f& _pos) {
			mWorldPos = _pos;
			mWorldTrans[0][3] = _pos.x;
			mWorldTrans[1][3] = _pos.y;
			mWorldTrans[2][3] = _pos.z;
		}

		//--------------------------------------------------------------------------------------------------------------
		void TransformSrc::setLocalPos(const Vec3f& _pos) {
			mLocalPos = _pos;
		}

		//--------------------------------------------------------------------------------------------------------------
		void TransformSrc::setWorldRot(const Quatf& _rot) {
			mWorldRot = _rot;
			mWorldTrans = Mat34f(_rot, mWorldPos);
		}

		//--------------------------------------------------------------------------------------------------------------
		void TransformSrc::setLocalRot(const Quatf& _rot) {
			mLocalRot = _rot;
		}

	}	// namespace core
}	// namespace rev