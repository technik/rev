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
			//layout(location = 1) in vec3 normal;
			//layout(location = 2) in float material;

			//layout(location = 0) uniform mat4 uWVP;

			//layout(location = 0) out float light;
			//layout(location = 1) out float vMat;
			
			void main ( void )
			{
				gl_Position = uWVP * vec4(vertex, 1.0);
				vMat = material;
				light = min(max(dot(normal, normalize(vec3(0.1, -0.3, 0.8))), 0.0)+0.3, 1.0);
			}
#endif
#ifdef PXL_SHADER
			//layout(location = 0) in float light;
			//layout(location = 1) in float vMat;

			//vec3 material[4] = { vec3(0.7,0.5,0.2), vec3(0.0,0.5,0.0), vec3(0.0,0.5,0.0), vec3(0.4, 0.4, 0.4) };

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