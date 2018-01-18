//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include "debugGUI.h"
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

namespace rev { namespace graphics { namespace gui {

	void init(const math::Vec2u& _windowSize) {
		ImGui_ImplGlfwGL3_Init();

		auto& io = ImGui::GetIO();
		io.DisplaySize.x = float(_windowSize.x());
		io.DisplaySize.y = float(_windowSize.y());
	}

	void startFrame() {
		ImGui_ImplGlfwGL3_NewFrame();
	}

	void finishFrame(float _dt) {
		// Setup time step
		auto& io = ImGui::GetIO();
		io.DeltaTime = _dt;

		ImGui::Begin("FPS");
		ImGui::Text("ms: %f2.2 fps: %d", _dt*1000, int(1.f/_dt));
		ImGui::End();

		// Render
		ImGui::Render();
	}

}}}