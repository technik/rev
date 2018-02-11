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
#include "editor.h"
#include "inspectors/MeshRendererInspector.h"
#include "inspectors/TransportInspector.h"

#include <graphics/debug/imgui.h>

namespace rev { namespace player {

	//----------------------------------------------------------------------------------------------
	void Editor::init(graphics::RenderScene& scene)
	{
		mTextures = {
			"textures/spnza_bricks_a_ddn.tga",
			"textures/sponza_curtain_blue_diff.tga",
			"textures/spnza_bricks_a_diff.tga",
			"textures/sponza_curtain_diff.tga",
			"textures/spnza_bricks_a_spec.tga"
		};
		mTextureMgr.init();
		createInspectors(scene);
	}

	//----------------------------------------------------------------------------------------------
	void Editor::update(game::Scene& scene)
	{
		// Show menu
		drawMainMenu();
		showProjectExplorer();
		showNodeTree(scene.root());
		showInspector();
		//ImGui::ShowDemoWindow();
	}

	//----------------------------------------------------------------------------------------------
	void Editor::pickTexture(std::shared_ptr<graphics::Texture>& texture)
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

	//----------------------------------------------------------------------------------------------
	void Editor::drawMainMenu() {
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

	//----------------------------------------------------------------------------------------------
	void Editor::showNodeTree(const game::SceneNode* root)
	{
		if(!mShowNodeTree)
			return;

		if(ImGui::Begin("Node Tree"))
		{
			// TODO: Make this a real tree
			for(auto& c : root->children())
			{
				ImGuiTreeNodeFlags node_flags = 
					ImGuiTreeNodeFlags_OpenOnArrow | 
					ImGuiTreeNodeFlags_OpenOnDoubleClick | 
					((mSelectedNode.lock() == c) ? ImGuiTreeNodeFlags_Selected : 0);
				bool node_open = ImGui::TreeNodeEx(c->name.c_str(), node_flags, c->name.c_str());
				if(ImGui::IsItemClicked())
					mSelectedNode = c;
				if(node_open)
				{
					ImGui::TreePop();
				}
			}
			ImGui::End();
		}
	}

	//----------------------------------------------------------------------------------------------
	void Editor::showInspector() {
		if(mShowInspector)
		{
			if(ImGui::Begin("Item Inspector"))
			{
				if(!mSelectedNode.expired())
				{
					auto inspectedNode = mSelectedNode.lock();
					ImGui::Text(inspectedNode->name.c_str());
					ImGui::Separator();
					for(auto& c : inspectedNode->components())
					{
						auto typeName = std::string(typeid(*c).name());
						auto i = mInspectors.find(typeName);
						if(i != mInspectors.end())
						{
							i->second->showInspectionPanel(c.get());
							ImGui::Separator();
						}
					}
				}
				ImGui::End();
			}
		}
	}

	//----------------------------------------------------------------------------------------------
	void Editor::showProjectExplorer() {
		if(mShowProjectExplorer)
		{
			if(ImGui::Begin("Project Explorer"))
			{
				for(auto& t: mTextures)
				{
					ImGui::Selectable(t.c_str());
				}
				ImGui::End();
			}
		}
	}

	//----------------------------------------------------------------------------------------------
	void Editor::createInspectors(graphics::RenderScene& scene) {
		mInspectors.insert(std::make_pair(
			std::string(typeid(game::MeshRenderer).name()),
			std::make_unique<RendererInspector>(mMaterials))
		);
		registerInspector<game::Transform,TransformInspector>();
	}

}}	// namespace rev::player