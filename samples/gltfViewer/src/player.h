//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <game/scene/sceneNode.h>
#include <game/application/base3dApplication.h>
#include <graphics/scene/camera.h>

#include "Renderer.h"

namespace rev {

	namespace game {
		class FlyBy;
		class Orbit;
	}

	class Player : public game::Base3dApplication
	{
	public:
		virtual std::string name() { return "gltf viewer"; }

		Player() = default;
		Player(const Player&) = delete;
		Player& operator=(const Player&) = delete;

	private:

		struct CommandLineOptions
		{
			std::string scene;
			std::string environment;
			float fov = 45.f;

			void registerOptions(core::CmdLineParser&);
		} m_options;

		// Extension interface
		// Life cycle
		void getCommandLineOptions(core::CmdLineParser&) override;
		bool init() override;
		void end() override;
		// Main loop
		bool updateLogic(TimeDelta logicDt) override;
		void render(TimeDelta renderDt) override;
		// Events
		void onResize() override;

		// Common components
		void updateUI(float dt);

	private:
		void loadScene(const std::string& scene);
		void createCamera();

		// Scene
		std::shared_ptr<game::SceneNode> m_sceneRoot;
		Renderer::SceneDesc m_sceneGraphics;
		float m_bgColor{};

		// Camera options
		gfx::Camera*						mFlybyCam = nullptr;
		game::FlyBy*						m_flyby;

		Renderer m_renderer;

	private:
		size_t m_sceneLoadStreamToken{};
	};

}	// namespace rev