//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#include "sceneNode.h"
#include "component.h"
#include <graphics/debug/debugGUI.h>
#include <graphics/debug/imgui.h>

using namespace rev::graphics;

namespace rev { namespace game {

	//------------------------------------------------------------------------------------------------------------------
	void SceneNode::init() {
		for(auto& c : mComponents)
			c->init();
	}

	//------------------------------------------------------------------------------------------------------------------
	void SceneNode::update(float _dt) {
		for(auto& c : mComponents)
			c->update(_dt);
	}

	//------------------------------------------------------------------------------------------------------------------
	void SceneNode::addComponent(Component* _c)
	{
		if(!_c)
			return;
		if(_c->node() != this) {
			_c->attachTo(this);
			mComponents.emplace_back(_c);
		}
	}

	//--------------------------------------------------------------------------------------------------------------
	void SceneNode::addChild(std::shared_ptr<SceneNode> child)
	{
		child->mParent = this;
		assert(std::find(mChildren.begin(), mChildren.end(), child) == mChildren.end());
		mChildren.push_back(child);
	}

}}	// namespace rev::game