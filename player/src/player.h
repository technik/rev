//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include "editor/editor.h"
#include <game/scene/scene.h>
#include <game/scene/ComponentLoader.h>
#include <graphics/backend/OpenGL/deviceOpenGL.h>
#include <graphics/renderer/ForwardRenderer.h>
#include <graphics/scene/camera.h>
#include <graphics/scene/renderScene.h>
#include <graphics/driver/geometryPool.h>

using namespace rev::editor;

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

	private:
		void createCamera();

		game::ComponentLoader					mComponentFactory;
		graphics::RenderScene					mGraphicsScene;
		game::Scene								mGameScene;
		const graphics::Camera*					mCamera;
		graphics::ForwardRenderer				mRenderer;
		gfx::DeviceOpenGL&						m_gfx;
		Editor									mGameEditor;
		std::unique_ptr<graphics::GeometryPool>	mGeometryPool;
	};

}	// namespace rev