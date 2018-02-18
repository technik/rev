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
#include "inspectors/MaterialInspector.h"
#include "inspectors/MeshRendererInspector.h"
#include "inspectors/TransportInspector.h"

#include <graphics/debug/imgui.h>

namespace rev { namespace editor {

	//----------------------------------------------------------------------------------------------
	void Editor::init(graphics::RenderScene& scene)
	{
		mTextureMgr.init();
		registerMaterials();
		createInspectors(scene);
	}

	//----------------------------------------------------------------------------------------------
	void Editor::update(game::Scene& scene)
	{
		// Show menu
		drawMainMenu();
		showMaterialExplorer();
		showNodeTree(scene.root());
		showInspector();
		//ImGui::ShowDemoWindow();
	}

	//----------------------------------------------------------------------------------------------
	void Editor::drawMainMenu() {
		ImGui::BeginMainMenuBar();
		if(ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("Open Project");
			ImGui::MenuItem("Save Project");
			ImGui::Separator();
			ImGui::MenuItem("Open Scene");
			ImGui::MenuItem("Save Scene");
			ImGui::Separator();
			ImGui::MenuItem("Import Asset");
			ImGui::EndMenu();
		}
		if(ImGui::BeginMenu("View"))
		{
			ImGui::MenuItem("Node Tree", "", &mShowNodeTree);
			ImGui::MenuItem("Inspector", "", &mShowInspector);
			ImGui::MenuItem("Project Explorer", "", &mShowProjectExplorer);
			ImGui::MenuItem("Material Explorer", "", &mShowMaterialExplorer);
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
				{
					mSelectedNode = c;
					mSelectedMaterial.reset();
				}
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
				if(!mSelectedMaterial.expired())
				{
					MaterialInspector mat(mTextures, mTextureMgr);
					mat.showInspectionPanel(*mSelectedMaterial.lock());
				}
				ImGui::End();
			}
		}
	}

	//----------------------------------------------------------------------------------------------
	void Editor::showMaterialExplorer() {
		if(mShowMaterialExplorer)
		{
			if(ImGui::Begin("Material Explorer"))
			{
				for(auto& m: mMaterials)
				{
					if(ImGui::Selectable(m->name.c_str()))
					{
						mSelectedMaterial = m;
						mSelectedNode.reset();
					}
				}
				ImGui::Separator();
				if(ImGui::Button("+"))
				{
					auto newMaterial = std::make_shared<Material>();
					mMaterials.push_back(newMaterial);
					newMaterial->name = "materials/new.mat";
					newMaterial->addParam(6, 0.5f); // Rougness
					newMaterial->addParam(7, 0.5f); // Metallic
					newMaterial->addTexture(5, mTextureMgr.get("errorTexture"));
				}
				ImGui::End();
			}
		}
	}

	//----------------------------------------------------------------------------------------------
	void Editor::registerMaterials()
	{
		// Register phony materials
		mTextures = {
			"textures/spnza_bricks_a_ddn.tga",
			"textures/sponza_curtain_blue_diff.tga",
			"textures/spnza_bricks_a_diff.tga",
			"textures/sponza_curtain_diff.tga",
			"textures/spnza_bricks_a_spec.tga"
		};
		const std::string materialList[] = 
		{
			"materials/red_curtain.mat",
			"materials/blue_curtain.mat",
			"materials/green_curtain.mat",
			"materials/bricks.mat"
		};
		for(auto& file : materialList)
		{
			auto material = std::make_shared<Material>();
			material->load(file);
			mMaterials.push_back(material);
		}
		mSelectedMaterial = mMaterials[0];
	}

	//----------------------------------------------------------------------------------------------
	void Editor::createInspectors(graphics::RenderScene& scene) {
		mInspectors.insert(std::make_pair(
			std::string(typeid(game::MeshRenderer).name()),
			std::make_unique<RendererInspector>(mMaterials))
		);
		registerInspector<game::Transform,TransformInspector>();
	}

}}	// namespace rev::editor