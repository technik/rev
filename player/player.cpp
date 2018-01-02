//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#include "player.h"

namespace rev {

	//------------------------------------------------------------------------------------------------------------------
	bool Player::init(Window _window) {
		assert(!mGfxDriver);

		mGfxDriver = graphics::GraphicsDriverGL::createDriver(_window);
		if(mGfxDriver) {
			glClearColor(0.f,0.5f,1.f,1.f);
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
			out vec3 color;
			void main (void) {
				
				color = float3(0.0,1.0,0.0);
			}
#endif
				)"
			);
		}
		return mGfxDriver != nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Player::update() {
		if(!mGfxDriver)
			return true;

		
		
		glClear(GL_COLOR_BUFFER_BIT);
		mGfxDriver->swapBuffers();
		return true;
	}

}	// namespace rev