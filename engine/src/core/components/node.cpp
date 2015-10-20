//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Transform source for the component system
#include "node.h"

namespace rev {
	namespace core {

		//--------------------------------------------------------------------------------------------------------------
		void Node::move(const math::Vec3f& _translation) {
			setPosition(position() + _translation, TransformSrc::global);
		}

		//--------------------------------------------------------------------------------------------------------------
		void Node::moveLocal(const math::Vec3f& _translation) {
			setPosition(position() + _translation, TransformSrc::local);
		}

		//--------------------------------------------------------------------------------------------------------------
		void Node::setPos(const math::Vec3f& _pos) {
			setPosition(_pos, TransformSrc::global);
		}

		//--------------------------------------------------------------------------------------------------------------
		void Node::setPosLocal(const math::Vec3f& _pos) {
			setPosition(_pos, TransformSrc::local);
		}

		//--------------------------------------------------------------------------------------------------------------
		void Node::setPos(const math::Vec3f& _pos) {
			setPosition(_pos, TransformSrc::global);
		}

		//--------------------------------------------------------------------------------------------------------------
		void Node::addComponent(Component* _c) {
			mComponents.insert(_c);
		}

		//--------------------------------------------------------------------------------------------------------------
		void Node::removeComponent(Component* _c) {
			mComponents.erase(_c);
		}

	}
}