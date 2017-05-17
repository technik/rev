//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Transform source for the component system
#include "sceneNode.h"
#include <core/components/component.h>

namespace rev {
	namespace core {

		//--------------------------------------------------------------------------------------------------------------
		void SceneNode::move(const math::Vec3f& _translation) {
			setPosition(position() + _translation, TransformSrc::global);
		}

		//--------------------------------------------------------------------------------------------------------------
		void SceneNode::moveLocal(const math::Vec3f& _translation) {
			setPosition(position() + transform().rotate(_translation), TransformSrc::local);
		}

		//--------------------------------------------------------------------------------------------------------------
		void SceneNode::setPos(const math::Vec3f& _pos) {
			setPosition(_pos, TransformSrc::global);
		}

		//--------------------------------------------------------------------------------------------------------------
		void SceneNode::setPosLocal(const math::Vec3f& _pos) {
			setPosition(_pos, TransformSrc::local);
		}

		//--------------------------------------------------------------------------------------------------------------
		void SceneNode::setRot(const math::Quatf& _rot) {
			setRotation(_rot, TransformSrc::global);
		}

		//--------------------------------------------------------------------------------------------------------------
		void SceneNode::setRotLocal(const math::Quatf& _rot) {
			setRotation(_rot, TransformSrc::local);
		}

		//------------------------------------------------------------------------------------------------------------------
		void SceneNode::setTransform(const math::Mat34f& _m) {
			TransformSrc::setTransform(_m, TransformSrc::global);
		}

		//------------------------------------------------------------------------------------------------------------------
		void SceneNode::rotate(const math::Vec3f& _axis, float _angle)
		{
			math::Quatf turn = math::Quatf(_axis, _angle);
			setRotation(turn * rotation(), TransformSrc::global);
		}

		//--------------------------------------------------------------------------------------------------------------
		void SceneNode::addComponent(Component* _c) {
			if(!_c)
				return;
			if(_c->node() != this) {
				_c->attachTo(this);
				mComponents.push_back(_c);
			}
		}

		//--------------------------------------------------------------------------------------------------------------
		void SceneNode::removeComponent(Component* _c) {
			assert(_c);
			if(_c->node()) {
				_c->dettach();
				for (auto c = mComponents.begin(); c != mComponents.end(); ++c) {
					if (*c == _c) {
						mComponents.erase(c);
						return;
					}
				}
			}
		}

	}
}