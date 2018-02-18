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
#include <game/scene/transform/transform.h>
#include <graphics/debug/debugGUI.h>
#include <graphics/debug/imgui.h>

namespace rev { namespace editor {

	struct TransformInspector : Editor::ComponentInspector
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

}}	// namespace rev::editor
