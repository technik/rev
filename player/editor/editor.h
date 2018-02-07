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
#pragma once
#include <game/scene/sceneNode.h>
#include <graphics/debug/debugGUI.h>
#include <graphics/debug/imgui.h>
#include <game/textureManager.h>
#include <game/scene/scene.h>
#include <string>
#include <vector>

namespace rev { namespace player {

	using namespace rev::graphics;

	class Editor {
	public:
		void init()
		{
			mTextures = {
				"textures/spnza_bricks_a_ddn.tga",
				"textures/sponza_curtain_blue_diff.tga",
				"textures/spnza_bricks_a_diff.tga",
				"textures/sponza_curtain_diff.tga",
				"textures/spnza_bricks_a_spec.tga"
			};
			mTextureMgr.init();
		}

		void update(game::Scene& scene)
		{
			// Show menu
			drawMainMenu();
			showProjectExplorer();
			//showNodeTree(scene.nodes());
			//showInspector(scene.nodes());
			//ImGui::ShowDemoWindow();
		}
		
		void pickTexture(std::shared_ptr<graphics::Texture>& texture)
		{
			if (ImGui::Button("Select.."))
				ImGui::OpenPopup("select texture");
			ImGui::SameLine();
			ImGui::TextUnformatted("<None>");
			if (ImGui::BeginPopup("select texture"))
			{
				for (auto& t : mTextures)
					if (ImGui::Selectable(t.c_str()))
						texture = mTextureMgr.get(t);
				ImGui::EndPopup();
			}
		}

	private:
		void drawMainMenu() {
			ImGui::BeginMainMenuBar();
			if(ImGui::BeginMenu("View"))
			{
				ImGui::MenuItem("Node Tree", "", &mShowNodeTree);
				ImGui::MenuItem("Inspector", "", &mShowInspector);
				ImGui::MenuItem("Project Explorer", "", &mShowProjectExplorer);
				ImGui::MenuItem("Render Options", "", &mShowRenderOptions);
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		/*void showNodeTree(const std::vector<game::SceneNode*>& _nodes)
		{
			if(!mShowNodeTree)
				return;

			if(ImGui::Begin("Node Tree"))
			{
				gui::showList("Node list", mSelectedNodeNdx, [=](size_t i){ return _nodes[i]->name.c_str(); }, _nodes.size());
				if(mSelectedNodeNdx >= 0)
					mShowInspector = true;
				ImGui::End();
			}
		}

		class Inspectable {
		public:
			Inspectable(game::SceneNode& node)
				: mNode(node)
			{
				mMesh = node.component<game::MeshRenderer>();
			}

			void showInspectorMenu(Editor& editor)
			{
				mNode.showDebugInfo();
				if(mMesh)
				{
					editor.pickTexture(mMesh->material().albedo);
				}
			}

			game::SceneNode& mNode;
			game::MeshRenderer* mMesh = nullptr;
		};

		void showInspector(std::vector<game::SceneNode*>& _nodes) {
			if(mShowInspector)
			{
				if(ImGui::Begin("Item Inspector"))
				{
					if((mSelectedNodeNdx >= 0))
					{
						Inspectable element(*_nodes[mSelectedNodeNdx]);
						element.showInspectorMenu(*this);
					}
					ImGui::End();
				}
			}
		}*/

		void showProjectExplorer() {
			if(mShowProjectExplorer)
			{
				if(ImGui::Begin("Project Explorer"))
				{
					//ImGui::TreeNode("Textures");
					for(auto& t: mTextures)
					{
						ImGui::Selectable(t.c_str());
					}
					//ImGui::TreePop();
					ImGui::End();
				}
			}
		}

		std::vector<std::string>	mTextures;
		game::TextureManager mTextureMgr;

		bool mShowInspector = false;
		bool mShowProjectExplorer = true;
		bool mShowRenderOptions = false;
		bool mShowNodeTree = false;
		int mSelectedNodeNdx = -1;
	};

}}
