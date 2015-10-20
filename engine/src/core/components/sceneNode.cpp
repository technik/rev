//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Transform source for the component system
#include "sceneNode.h"

namespace rev {
	namespace core {

		//--------------------------------------------------------------------------------------------------------------
		void SceneNode::move(const math::Vec3f& _translation) {
			setPosition(position() + _translation, TransformSrc::global);
		}

		//--------------------------------------------------------------------------------------------------------------
		void SceneNode::moveLocal(const math::Vec3f& _translation) {
			setPosition(position() + _translation, TransformSrc::local);
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
		void SceneNode::addComponent(Component* _c) {
			mComponents.insert(_c);
		}

		//--------------------------------------------------------------------------------------------------------------
		void SceneNode::removeComponent(Component* _c) {
			mComponents.erase(_c);
		}

	}
}