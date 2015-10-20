//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Base component in the component system
#include "component.h"
#include "sceneNode.h"

namespace rev {
	namespace core {
		//----------------------------------------------------------------------------------------------------------------------
		Component::~Component() {
			dettach();
		}

		//----------------------------------------------------------------------------------------------------------------------
		void Component::attachTo(SceneNode* _node) {
			dettach(); // Dettach from previous parent, if any.
			mNode = _node;
			_node->addComponent(this);
		}

		//----------------------------------------------------------------------------------------------------------------------
		void Component::dettach() {
			if(mNode) {
				SceneNode* oldNode = mNode;
				mNode = nullptr;
				oldNode->removeComponent(this);
			}
		}
	}
}