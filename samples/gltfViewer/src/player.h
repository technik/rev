//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <game/scene/scene.h>
#include <game/scene/ComponentLoader.h>
#include <graphics/backend/OpenGL/deviceOpenGL.h>
#include <graphics/renderer/ForwardRenderer.h>
#include <graphics/scene/camera.h>
#include <graphics/scene/renderScene.h>
#include <graphics/driver/geometryPool.h>


namespace rev {

	class Player {
	public:
		Player(gfx::DeviceOpenGL& device)
			: m_gfx(device)
		{}

		Player(const Player&) = delete;
		Player& operator=(const Player&) = delete;

		bool init(const math::Vec2u& windowSize, const std::string& scene, const std::string& bg);
		void loadScene(const std::string& _assetFileName);

#ifdef _WIN32
		void onWindowResize(const math::Vec2u& _newSize);
#endif // _WIN32

		// Common components
		bool update();
		void updateUI(float dt);

	private:
		void createCamera();
		void createFloor();

		math::Vec2u m_windowSize;
		game::ComponentLoader				mComponentFactory;
		gfx::RenderScene					mGraphicsScene;
		game::Scene							mGameScene;
		const gfx::Camera*					mCamera;
		gfx::ForwardRenderer				mRenderer;
		gfx::DeviceOpenGL&					m_gfx;
		std::unique_ptr<gfx::GeometryPool>	mGeometryPool;
		std::shared_ptr<gfx::DirectionalLight>	m_envLight;
		std::shared_ptr<gfx::RenderObj>		m_floorGeom;

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