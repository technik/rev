//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <memory>
#include <graphics/driver/shader.h>
#include <graphics/driver/openGL/GraphicsDriverOpenGL.h>
#include <graphics/scene/renderGeom.h>

namespace rev {

	class Player {
	public:
		typedef rev::graphics::GraphicsDriverGL::NativeWindow	Window;
		bool init(Window);

		// Common components
		bool update();

	private:
		std::unique_ptr<graphics::Shader>		mShader;
		std::unique_ptr<graphics::RenderGeom>	mTriangle;
		graphics::GraphicsDriverGL* mGfxDriver = nullptr;
	};

}	// namespace rev