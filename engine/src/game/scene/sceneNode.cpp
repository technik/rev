//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2018 Carmelo J Fdez-Aguera
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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