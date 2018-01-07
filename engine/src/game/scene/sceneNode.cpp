//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#include "sceneNode.h"
#include "component.h"

namespace rev { namespace game {

	//------------------------------------------------------------------------------------------------------------------
	void SceneNode::update(float _dt) {
		for(auto c : mComponents)
			c->update(_dt);
	}

	//------------------------------------------------------------------------------------------------------------------
	void SceneNode::addComponent(Component* _c) {
		if(!_c)
			return;
		if(_c->node() != this) {
			_c->attachTo(this);
			mComponents.push_back(_c);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
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

}}	// namespace rev::game