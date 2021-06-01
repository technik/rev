//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <game/scene/sceneNode.h>
#include <game/application/base3dApplication.h>
#include <gfx/scene/camera.h>
#include <gfx/renderer/RasterScene.h>
#include <gfx/renderer/deferred/DeferredRenderer.h>

namespace rev {

	namespace game {
		class FlyBy;
		class Orbit;
	}

	class Pluto : public game::Base3dApplication
	{
	public:
		virtual std::string name() { return "gltf viewer"; }

		Pluto() = default;
		Pluto(const Pluto&) = delete;
		Pluto& operator=(const Pluto&) = delete;

	private:

		struct CommandLineOptions
		{
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
		void createCamera();

		// Scene
		std::shared_ptr<game::SceneNode> m_sceneRoot;
		gfx::DeferredRenderer::SceneDesc m_sceneGraphics;
		float m_bgColor{};

		// Camera options
		gfx::Camera*						mFlybyCam = nullptr;
		game::FlyBy*						m_flyby;

		gfx::DeferredRenderer m_renderer;
	};

}	// namespace rev