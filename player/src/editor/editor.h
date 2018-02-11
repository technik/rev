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
		void init(graphics::RenderScene& scene);

		void update(game::Scene& scene);

		struct ComponentInspector {
			virtual void showInspectionPanel(game::Component*) const = 0;
		};

	private:
		void pickTexture(std::shared_ptr<graphics::Texture>& texture);
		void drawMainMenu();
		void showNodeTree(const game::SceneNode* root);
		void showInspector();
		void showProjectExplorer();

		

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

		template<class Inspected, class Inspector>
		void registerInspector()
		{
			mInspectors.insert(std::make_pair(
				std::string(typeid(Inspected).name()),
				std::make_unique<Inspector>())
			);
		}

		void createInspectors (graphics::RenderScene& scene) {
			mInspectors.insert(std::make_pair(
				std::string(typeid(game::MeshRenderer).name()),
				std::make_unique<RendererInspector>(scene))
			);
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
