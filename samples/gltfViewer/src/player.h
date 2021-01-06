//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <game/scene/scene.h>
#include <game/application/base3dApplication.h>
#include <graphics/backend/rasterPipeline.h>
#include <graphics/backend/OpenGL/deviceOpenGL.h>
#include <graphics/renderer/ForwardRenderer.h>
#include <graphics/renderer/deferred/DeferredRenderer.h>
#include <graphics/scene/camera.h>
#include <graphics/scene/renderScene.h>
#include <graphics/driver/geometryPool.h>


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
		void createFloor();
		void updateSceneBBox();

		// Vulkan objects to move into rev::gfx
		vk::Semaphore m_imageAvailableSemaphore;
		vk::RenderPass m_uiPass;
		vk::PipelineLayout m_uiPipelineLayout;
		std::unique_ptr<gfx::RasterPipeline> m_uiPipeline;

		// Scene
		gfx::RenderScene					mGraphicsScene;
		game::Scene							mGameScene;
		math::AABB							m_globalBBox;

		// Camera options
		gfx::Camera*						mFlybyCam = nullptr;
		game::FlyBy*						m_flyby;

		// Renderer
		gfx::DeferredRenderer				mDeferred;
		std::shared_ptr<gfx::DirectionalLight>	m_envLight;
		std::shared_ptr<gfx::RenderObj>		m_floorGeom;
		std::shared_ptr<game::SceneNode>	m_gltfRoot;

	private:

		struct ViewerState
		{
			bool floor = true;
		};

		struct BgOptions
		{
			bool shadows = true;
			float elevation = math::Constants<float>::halfPi*0.4;
			float rotation = 0.f;
			math::Vec3f shadowDir;
		} m_bgOptions;

		struct PostFX
		{
			bool bloom = false;
			float bloomThreshold = 1.f;
		};

		struct RenderOptions
		{
			float EV = 0.f;
			PostFX postOptions;
		};
	};

}	// namespace rev