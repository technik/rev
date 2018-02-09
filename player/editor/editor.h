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
#include <game/scene/ComponentLoader.h>
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
			showNodeTree(scene.root());
			showInspector();
			//ImGui::ShowDemoWindow();
		}

		void registerCustomFactories(game::ComponentLoader& loader);
		
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

		void showNodeTree(const game::SceneNode* root)
		{
			if(!mShowNodeTree)
				return;

			if(ImGui::Begin("Node Tree"))
			{
				// TODO: Make this a real tree
				for(auto c : root->children())
				{
					if(ImGui::Selectable(c->name.c_str()))
					{
						mSelectedNode = c;
					}
				}
				ImGui::End();
			}
		}

		struct ComponentInspector {
			virtual void showInspectionPanel() const = 0;
		};

		struct RendererInspector : ComponentInspector
		{
			void showInspectionPanel() const override {
				ImGui::Text("I'm a renderable");
			}
		};

		void showInspector() {
			if(mShowInspector)
			{
				if(ImGui::Begin("Item Inspector"))
				{
					/*if((mSelectedNodeNdx >= 0))
					{
						Inspectable element(*_nodes[mSelectedNodeNdx]);
						element.showInspectorMenu(*this);
					}*/
					ImGui::End();
				}
			}
		}

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

		bool mShowInspector = true;
		bool mShowProjectExplorer = false;
		bool mShowRenderOptions = false;
		bool mShowNodeTree = true;
		std::weak_ptr<game::SceneNode>	mSelectedNode;
	};

	// Inline implementation
	inline void Editor::registerCustomFactories(game::ComponentLoader& loader)
	{
		//
	}

}}
