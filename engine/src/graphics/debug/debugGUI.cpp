//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include "debugGUI.h"
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include <core/tools/log.h>
#include <input/pointingInput.h>
#include <input/keyboard/keyboardInput.h>

namespace rev::gfx { namespace gui {

	//------------------------------------------------------------------------------------------------------------------
	void init(const math::Vec2u& _windowSize) {
		ImGui_ImplGlfwGL3_Init();

		auto& io = ImGui::GetIO();
		io.DisplaySize.x = float(_windowSize.x());
		io.DisplaySize.y = float(_windowSize.y());
#ifdef _WIN32
		auto keyboard = input::KeyboardInput::get();
		if(keyboard)
		{
			io.KeyMap[ImGuiKey_Tab] = (int)input::KeyboardInput::Key::Tab;
			io.KeyMap[ImGuiKey_LeftArrow] = (int)input::KeyboardInput::Key::KeyLeft;
			io.KeyMap[ImGuiKey_RightArrow] = (int)input::KeyboardInput::Key::KeyRight;
			io.KeyMap[ImGuiKey_UpArrow] = (int)input::KeyboardInput::Key::KeyUp;
			io.KeyMap[ImGuiKey_DownArrow] = (int)input::KeyboardInput::Key::KeyDown;
			io.KeyMap[ImGuiKey_Delete] = (int)input::KeyboardInput::Key::Delete;
			io.KeyMap[ImGuiKey_Backspace] = (int)input::KeyboardInput::Key::BackSpace;
			io.KeyMap[ImGuiKey_Enter] = (int)input::KeyboardInput::Key::Enter;
			io.KeyMap[ImGuiKey_Escape] = (int)input::KeyboardInput::Key::Escape;
			io.KeyMap[ImGuiKey_A] = 'A';
			io.KeyMap[ImGuiKey_C] = 'C';
			io.KeyMap[ImGuiKey_V] = 'V';
			io.KeyMap[ImGuiKey_X] = 'X';
			keyboard->onPress([&](int key) {
				io.KeysDown[key] = true;
				if(key >= ' ' && key <= '~')
					io.AddInputCharacter(key);
			});
			keyboard->onRelease([&](int key) {
				io.KeysDown[(int)key] = false;
			});
		}
#endif // _WIN32

	}

	//------------------------------------------------------------------------------------------------------------------
	void startFrame(const math::Vec2u& _windowSize) {
		// Setup Mouse input
		auto& io = ImGui::GetIO();
		io.DisplaySize.x = float(_windowSize.x());
		io.DisplaySize.y = float(_windowSize.y());
		auto pointingInput = input::PointingInput::get();
		if(pointingInput)
		{
			io.MouseDown[0] = pointingInput->leftDown();
#ifdef _WIN32
			io.MouseDown[1] = pointingInput->rightDown();
			io.MouseDown[2] = pointingInput->middleDown();
#endif
			io.MousePos = { 
				(float)pointingInput->touchPosition().x(),
				(float)pointingInput->touchPosition().y()
			};
		}

		// Begin frame
		ImGui_ImplGlfwGL3_NewFrame();
	}

	void drawFPS(float _dt)
	{
		constexpr size_t nSamples = 50;
		static float average[nSamples] = {};
		static int ndx = 0;
		average[ndx++] = _dt;
		ndx %= nSamples;
		float dt = 0;
		for(int i = 0; i < nSamples; ++i)
			dt += average[i];
		dt /= nSamples;
		const float DISTANCE = 10.f;
		ImVec2 window_pos = ImVec2(ImGui::GetIO().DisplaySize.x - DISTANCE, ImGui::GetIO().DisplaySize.y - DISTANCE);
		ImVec2 window_pos_pivot = ImVec2(1.0f, 1.0f);
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.3f)); // Transparent background
		bool isOpen = true;
		if (ImGui::Begin("FPS Counter", &isOpen, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings))
		{
			ImGui::Text("Performance");
			ImGui::Separator();
			ImGui::Text("ms: %f2.2 fps: %d", dt*1000, int(1.f/dt));
			ImGui::End();
		}
		ImGui::PopStyleColor();
	}

	//------------------------------------------------------------------------------------------------------------------
	void finishFrame(float _dt) {
		// Setup time step
		auto& io = ImGui::GetIO();
		io.DeltaTime = _dt;

		drawFPS(_dt);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool beginWindow(const char* _name) {
		return ImGui::Begin(_name, nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	}

	//------------------------------------------------------------------------------------------------------------------
	void endWindow() {
		ImGui::End();
	}

	//------------------------------------------------------------------------------------------------------------------
	void showCombo(const char* title, int& selected, std::function<const char*(size_t)> items, size_t nItems)
	{
		if(ImGui::BeginCombo(title, nullptr))
		{
			for(size_t i = 0; i < nItems; ++i)
			{
				if(ImGui::Selectable(items(i)))
					selected = i;
				if(i == selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void showList(const char* title, int& selected, std::function<const char*(size_t)> items, size_t nItems)
	{
		if(ImGui::TreeNode(title))
		{
			for(size_t i = 0; i < nItems; ++i)
			{
				if(ImGui::Selectable(items(i)))
					selected = i;
			}
			ImGui::TreePop();
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void text(const char* _text)
	{
		ImGui::Text("%s", _text);
	}

	//------------------------------------------------------------------------------------------------------------------
	void slider(const char* _name, float& _val, float _min, float _max)
	{
		ImGui::SliderFloat(_name, &_val, _min, _max);
	}

	//------------------------------------------------------------------------------------------------------------------
	void colorPicker(const char* name, math::Vec3f& _clr)
	{
		ImGui::ColorEdit3(name, _clr.data());
	}

}}