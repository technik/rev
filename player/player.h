//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <graphics/driver/openGL/GraphicsDriverOpenGL.h>
#include <graphics/scene/renderGeom.h>
#include <game/scene/renderScene.h>
#include <game/scene/sceneNode.h>
#include <graphics/scene/camera.h>
#include <graphics/renderer/ForwardRenderer.h>
#include <graphics/driver/texture.h>
#include "gameProject.h"
#include "editor/editor.h"
#include <game/scene/scene.h>

using namespace rev::player;

namespace rev {

	class Player {
	public:
		Player()
			: mGameScene(mGraphicsScene)
		{}

		Player(const Player&) = delete;
		Player& operator=(const Player&) = delete;

		typedef rev::graphics::GraphicsDriverGL::NativeWindow	Window;
		bool init(Window);
		void loadScene(const char* assetFileName);

		// Common components
		bool update();

	private:
		void createCamera();

		std::shared_ptr<graphics::Texture>		mXORTexture;

		game::RenderScene						mGraphicsScene;
		game::Scene								mGameScene;
		const graphics::Camera*					mCamera;
		graphics::ForwardRenderer				mRenderer;
		graphics::GraphicsDriverGL*				mGfxDriver = nullptr;
		Project									mGameProject;
		Editor									mGameEditor;
	};

}	// namespace rev