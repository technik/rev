//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Transform source for the component system
#include "sceneNode.h"
#include <core/components/component.h>

namespace rev {
	namespace core {

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