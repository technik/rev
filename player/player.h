//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <memory>
#include <graphics/driver/shader.h>
#include <graphics/driver/openGL/GraphicsDriverOpenGL.h>
#include <graphics/scene/renderGeom.h>
#include <game/scene/renderScene.h>
#include <game/scene/sceneNode.h>

namespace rev {

	class Player {
	public:
		typedef rev::graphics::GraphicsDriverGL::NativeWindow	Window;
		bool init(Window);

		// Common components
		bool update();

	private:
		float t = 0.f;
		math::Mat44f							mProjectionMtx;
		std::unique_ptr<graphics::Shader>		mShader;
		std::unique_ptr<graphics::RenderGeom>	mTriangleGeom;
		game::SceneNode*						mTriangle;
		game::RenderScene						mGraphicsScene;
		graphics::GraphicsDriverGL*				mGfxDriver = nullptr;
	};

}	// namespace rev