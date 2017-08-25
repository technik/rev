//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Transform source for the component system
#include "sceneNode.h"
#include <game/scene/component.h>

namespace rev {
	namespace game {

		//--------------------------------------------------------------------------------------------------------------
		void SceneNode::init() {
			// init components
			for(auto c : mComponents)
				c->onCreate();
			for(auto c : mComponents)
				c->init();
			// init children
			for(auto c : mChildren)
				c->init();
		}

		//--------------------------------------------------------------------------------------------------------------
		void SceneNode::update() {
			// Update components
			for(auto c : mComponents)
				c->update();
			// Update children
			for(auto c : mChildren)
				c->update();
		}

		//--------------------------------------------------------------------------------------------------------------
		void SceneNode::attachTo(SceneNode* _parent) {
			// Dettach from old parent
			if(mParent) {
				for(auto i = mParent->mChildren.begin(); i != mParent->mChildren.end(); ++i) {
					if(*i == this) {
						mParent->mChildren.erase(i);
						break;
					}
				}
			}
			// Attach to new parent (if any)
			if(_parent) {
				_parent->mChildren.push_back(this);
			}
			mParent = _parent;
		}

		//--------------------------------------------------------------------------------------------------------------
		void SceneNode::addComponent(Component* _c) {
			if(!_c)
				return;
			mComponents.push_back(_c);
		}

	} // namespace core
}