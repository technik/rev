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
		void SceneNode::addComponent(Component* _c) {
			if(!_c)
				return;
			mComponents.push_back(_c);
		}

	} // namespace core
}