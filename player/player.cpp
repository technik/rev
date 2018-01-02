//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#include "player.h"
#include <math/algebra/vector.h>

namespace rev {

	const std::vector<math::Vec2f> vertices = {
		{1.f, 1.f},
		{-1.f, 1.f},
		{0.f,-1.f}
	};
	const std::vector<uint16_t> indices = { 0, 1, 2};

	//------------------------------------------------------------------------------------------------------------------
	bool Player::init(Window _window) {
		assert(!mGfxDriver);

		mGfxDriver = graphics::GraphicsDriverGL::createDriver(_window);
		if(mGfxDriver) {
			glClearColor(0.f,0.5f,1.f,1.f);
			// Create shader
			mShader = graphics::Shader::createShader(
				R"(
#ifdef VTX_SHADER
			layout(location = 0) in vec2 vertex;
			
			void main ( void )
			{
				gl_Position = vec4(vertex, 0.0, 1.0);
			}
#endif
#ifdef PXL_SHADER
			out lowp vec3 color;
			void main (void) {
				
				color = vec3(0.0,1.0,0.0);
			}
#endif
				)"
			);
			if(mShader)
				mShader->bind();

			mTriangle = std::make_unique<graphics::RenderGeom>(vertices,indices);
		}
		return mGfxDriver != nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Player::update() {
		if(!mGfxDriver)
			return true;

		glClear(GL_COLOR_BUFFER_BIT);

		mTriangle->render();

		mGfxDriver->swapBuffers();

		return true;
	}

}	// namespace rev