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

		// Static data
		DataConstructible<TransformSrc>::FactoryMap DataConstructible<TransformSrc>::sFactories;

		//--------------------------------------------------------------------------------------------------------------
		TransformSrc::~TransformSrc() {
			for(auto child : mChildren)
				delete child;
		}

		//--------------------------------------------------------------------------------------------------------------
		Mat34f TransformSrc::worldTransform() const {
			Mat34f wt = mLocalTransform;
			TransformSrc* parent = mParent;
			while (parent) {
				wt = parent->mLocalTransform * wt; // Update matrix to parent coordinates
				parent = parent->mParent;
			}
			return wt;
		}

		//--------------------------------------------------------------------------------------------------------------
		void TransformSrc::attachTo(TransformSrc* _parent) {
			dettach();
			mParent = _parent;
			if (!mParent)
				return;
			Mat34f parentInvTransform = mParent->worldTransform().inverse();
			mLocalTransform = parentInvTransform * mLocalTransform;
			mParent->mChildren.push_back(this);
		}

		//--------------------------------------------------------------------------------------------------------------
		void TransformSrc::addChild(TransformSrc* _child) {
			_child->attachTo(this);
		}

		//--------------------------------------------------------------------------------------------------------------
		void TransformSrc::dettach() {
			if (mParent)
			{
				// Remove from parent's children list
				for (auto i = mParent->mChildren.begin(); i != mParent->mChildren.end(); ++i)
				{
					if ((*i) == this)
					{
						mParent->mChildren.erase(i);
						break;
					}
				}
				// Collapse transform
				mLocalTransform = mParent->worldTransform() * mLocalTransform;
				mParent = nullptr;
			}
		}

		//--------------------------------------------------------------------------------------------------------------
		void TransformSrc::setPosition(const Vec3f& _pos) {
			mLocalTransform.setCol(3, _pos);
		}

		//--------------------------------------------------------------------------------------------------------------
		void TransformSrc::setRotation(const Quatf& _rot) {
			mLocalTransform = Mat34f(_rot, position());
		}

		//--------------------------------------------------------------------------------------------------------------
		void TransformSrc::setTransform(const Mat34f& _trans) {
			mLocalTransform = _trans;
		}

		//--------------------------------------------------------------------------------------------------------------
		void TransformSrc::setWorldPosition(const Vec3f& _pos) {
			if (mParent)
				setPosition(mParent->worldTransform().inverse() * _pos);
			else
				setPosition(_pos);
		}

		//--------------------------------------------------------------------------------------------------------------
		void TransformSrc::setWorldRotation(const Quatf& _rot) {
			if(mParent) {
				Mat34f worldTrans(_rot, worldPosition());
				setTransform(mParent->transform().inverse() * worldTrans);
			}
			else
				setRotation(_rot);
		}

		//--------------------------------------------------------------------------------------------------------------
		void TransformSrc::setWorldTransform(const math::Mat34f& _transform) {
			if(mParent)
				setTransform(mParent->transform().inverse() * _transform);
			else
				setTransform(_transform);
		}

	}	// namespace core
}	// namespace rev