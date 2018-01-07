//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#include "player.h"
#include <math/algebra/vector.h>

using namespace rev::math;
using namespace rev::graphics;

namespace rev {

	const std::vector<Vec3f> vertices = {
		{1.f, 1.f, 0.f},
		{-1.f, 1.f, 0.f},
		{0.f,-1.f, 0.f}
	};
	const std::vector<uint16_t> indices = { 0, 1, 2};

	//------------------------------------------------------------------------------------------------------------------
	bool Player::init(Window _window) {
		assert(!mGfxDriver);

		mGfxDriver = GraphicsDriverGL::createDriver(_window);
		if(mGfxDriver) {
			glClearColor(89.f/255.f,235.f/255.f,1.f,1.f);
			// Create shader
			mShader = Shader::createShader(
				R"(
#ifdef VTX_SHADER
			layout(location = 0) in vec3 vertex;
			layout(location = 0) uniform mat4 uWorldViewProjection;
			
			void main ( void )
			{
				gl_Position = uWorldViewProjection * vec4(vertex, 1.0);
			}
#endif
#ifdef PXL_SHADER
			out lowp vec3 color;
			void main (void) {
				
				color = vec3(255.0/255.0,22.0/255.f,88.0/255.0);
			}
#endif
				)"
			);
			if(mShader)
				mShader->bind();

			mTriangleGeom = std::make_unique<graphics::RenderGeom>(vertices,indices);
			mTriangle.model = mTriangleGeom.get();
			mTriangle.transform = AffineTransform::identity();
			mTriangle.transform.matrix()(0,0) = -0.25f;
			mTriangle.transform.matrix()(0,1) = 0.f;
			mTriangle.transform.matrix()(1,0) = 0.f;
			mTriangle.transform.matrix()(1,1) = -0.25f;
		}
		return mGfxDriver != nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Player::update() {
		if(!mGfxDriver)
			return true;

		glClear(GL_COLOR_BUFFER_BIT);
		
		auto vp = Mat44f::identity();
		auto worldMatrix = Mat44f::identity();

		// For each render obj
		worldMatrix.block<3,4>(0,0) = mTriangle.transform.matrix();
		glUniformMatrix4fv(0, 1, !Mat44f::is_col_major, (vp*worldMatrix).data());
		mTriangle.model->render();

		mGfxDriver->swapBuffers();

		return true;
	}

}	// namespace rev