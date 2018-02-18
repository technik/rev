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
#include "../editor.h"
#include <game/scene/meshRenderer.h>
#include <graphics/scene/renderObj.h>
#include <graphics/scene/material.h>
#include <graphics/debug/imgui.h>

namespace rev { namespace editor {

	struct RendererInspector : Editor::ComponentInspector
	{
		RendererInspector(const std::vector<std::string>& materials)
			: mMaterials(materials)
		{}

		void showInspectionPanel(game::Component*c) const override {
			auto meshRenderer = static_cast<game::MeshRenderer*>(c);
			ImGui::Text("Materials:");
			auto& renderObj = meshRenderer->renderObj();
			int i = 0;
			for(auto& mat : renderObj.materials)
			{
				auto tag = std::to_string(i++);
				std::string matName = mat?mat->name : std::string("---");

				if(ImGui::BeginCombo(tag.c_str(), matName.c_str()))
				{
					for(auto& m : mMaterials)
					{
						if(ImGui::Selectable(m.c_str(), mat->name==m))
						{
							// TODO: Request material using material manager
							//	mat = m;
						}
					}
					ImGui::EndCombo();
				}
			}
		}

		const std::vector<std::string>& mMaterials;
	};

}}	// namespace rev::editor