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
#include <game/scene/meshRenderer.h>
#include <game/scene/transform/transform.h>
#include <game/scene/ComponentLoader.h>
#include <string>
#include <typeinfo>
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
			createInspectors();
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

		struct ComponentInspector {
			virtual void showInspectionPanel(game::Component*) const = 0;
		};

		struct RendererInspector : ComponentInspector
		{
			void showInspectionPanel(game::Component*c) const override {
				auto meshRenderer = static_cast<game::MeshRenderer*>(c);
				ImGui::Text("Materials:");
				auto& renderObj = meshRenderer->renderObj();
				ImGuiID i = 0;
				for(auto mat : renderObj.materials)
				{
					ImGui::BeginChild(i);
					auto roughness = mat->floatParam(6);
					if(roughness)
						ImGui::SliderFloat("Roughness", roughness, 0.f, 1.f, "%.2f");
					auto metallic = mat->floatParam(7);
					if(metallic)
						ImGui::SliderFloat("Metallic", metallic, 0.f, 1.f, "%.2f");
					i++;
					ImGui::EndChild();
				}
			}
		};

		struct TransformInspector : ComponentInspector
		{
			void showInspectionPanel(game::Component*c) const override
			{
				auto tranformComp = static_cast<game::Transform*>(c);
				auto& xForm = tranformComp->xForm;
				graphics::gui::text("Transform");
				math::Vec3f pos = xForm.position();
				ImGui::InputFloat3("pos", pos.data());
				//gui::slider("angle", mCamAngle, -3.2f, 3.2f);
				graphics::gui::slider("x", pos.x(), -500.f, 500.f);
				graphics::gui::slider("y", pos.y(), -500.f, 500.f);
				graphics::gui::slider("z", pos.z(), 0.f, 600.f);
				xForm.position() = pos;
			}
		};

		void showInspector() {
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

		void showProjectExplorer() {
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

		template<class Inspected, class Inspector>
		void registerInspector()
		{
			mInspectors.insert(std::make_pair(
				std::string(typeid(Inspected).name()),
				std::make_unique<Inspector>())
			);
		}

		void createInspectors () {
			registerInspector<game::MeshRenderer,RendererInspector>();
			registerInspector<game::Transform,TransformInspector>();
		}

		std::vector<std::string>	mTextures;
		game::TextureManager mTextureMgr;

		bool mShowInspector = true;
		bool mShowProjectExplorer = false;
		bool mShowRenderOptions = false;
		bool mShowNodeTree = true;
		std::weak_ptr<game::SceneNode>	mSelectedNode;
		// TODO: This design can be improved
		std::map<std::string,std::unique_ptr<ComponentInspector>> mInspectors;
	};

}}
