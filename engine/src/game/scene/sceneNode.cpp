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
	void SceneNode::showDebugInfo() const
	{
		gui::text(name.c_str());
		for(auto& c : mComponents)
		{
			ImGui::Separator();
			c->showDebugInfo();
		}
	}

}}	// namespace rev::game