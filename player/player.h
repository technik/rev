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
#include "editor.h"

using namespace rev::player;

namespace rev {

	class Player {
	public:
		typedef rev::graphics::GraphicsDriverGL::NativeWindow	Window;
		bool init(Window);
		void loadScene(const char* assetFileName);

		// Common components
		bool update();

	private:
		void createCamera();

		std::shared_ptr<graphics::Texture>		mXORTexture;

		const graphics::Camera*					mCamera;
		graphics::ForwardRenderer				mRenderer;
		std::vector<graphics::RenderGeom>		mMeshes;
		std::vector<game::SceneNode>			mNodes;
		game::RenderScene						mGraphicsScene;
		graphics::GraphicsDriverGL*				mGfxDriver = nullptr;
		Project									mGameProject;
		Editor									mGameEditor;
	};

}	// namespace rev