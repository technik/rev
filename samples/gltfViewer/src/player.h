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

	namespace game {
		class FlyBy;
		class Orbit;
	}

	class Player {
	public:
		Player(gfx::DeviceOpenGL& device)
			: m_gfx(device)
		{}

		Player(const Player&) = delete;
		Player& operator=(const Player&) = delete;

		bool init(const math::Vec2u& windowSize, const std::string& scene, const std::string& bg);

#ifdef _WIN32
		void onWindowResize(const math::Vec2u& _newSize);
#endif // _WIN32

		// Common components
		bool update();
		void updateUI(float dt);

	private:
		void loadScene(const std::string& scene);
		void createCamera();
		void createFloor();

		math::Vec2u m_windowSize;
		game::ComponentLoader				mComponentFactory;
		gfx::RenderScene					mGraphicsScene;
		game::Scene							mGameScene;

		// Camera options
		const gfx::Camera*					mFlybyCam = nullptr;
		const gfx::Camera*					mOrbitCam = nullptr;
		game::FlyBy*						m_flyby;
		game::Orbit*						m_orbit;
		gfx::ForwardRenderer				mRenderer;
		gfx::DeviceOpenGL&					m_gfx;
		std::unique_ptr<gfx::GeometryPool>	mGeometryPool;
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