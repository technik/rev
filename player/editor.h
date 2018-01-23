//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2018 Carmelo J Fdez-Aguera
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#pragma once
#include <game/scene/sceneNode.h>
#include <graphics/debug/debugGUI.h>
#include <graphics/debug/imgui.h>
#include <vector>

namespace rev { namespace player {

	using namespace rev::graphics;

	class Editor {
	public:
		void update(const std::vector<game::SceneNode>& _nodes)
		{
			// Show menu
			drawMainMenu();
			showNodeTree(_nodes);
			showInspector(_nodes);
		}

	private:
		void drawMainMenu() {
			ImGui::BeginMainMenuBar();
			if(ImGui::BeginMenu("View"))
			{
				ImGui::MenuItem("Inspector", "", &mShowInspector);
				ImGui::MenuItem("Node Tree", "", &mShowNodeTree);
				ImGui::MenuItem("Render Options", "", &mShowRenderOptions);
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		void showNodeTree(const std::vector<game::SceneNode>& _nodes)
		{
			if(!mShowNodeTree)
				return;

			if(ImGui::Begin("Node Tree")) {
				gui::showList("Node list", mSelectedNodeNdx, [=](size_t i){ return _nodes[i].name.c_str(); }, _nodes.size());
				ImGui::End();
			}
		}

		void showInspector(const std::vector<game::SceneNode>& _nodes) {
			if(mShowInspector)
			{
				if(ImGui::Begin("Item Inspector"))
				{
					if((mSelectedNodeNdx >= 0))
						_nodes[mSelectedNodeNdx].showDebugInfo();
					ImGui::End();
				}
			}
		}

		bool mShowInspector = false;
		bool mShowRenderOptions = false;
		bool mShowNodeTree = false;
		int mSelectedNodeNdx = -1;
	};

}}
