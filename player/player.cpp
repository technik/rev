//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#include "player.h"
#include <math/algebra/vector.h>
#include <core/time/time.h>

using namespace rev::math;
using namespace rev::graphics;

namespace rev {

	const std::vector<Vec3f> vertices = {
		{1.f, 0.f, 1.f},
		{-1.f, 0.f, 1.f},
		{0.f,0.f, -1.f}
	};
	const std::vector<uint16_t> indices = { 0, 1, 2};

	//------------------------------------------------------------------------------------------------------------------
	bool Player::init(Window _window) {
		core::Time::init();

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

			mProjectionMtx = math::frustrumMatrix(0.8f, 4.f/3.f,0.1f,1000.f);
			mTriangleGeom = std::make_unique<graphics::RenderGeom>(vertices,indices);
			mTriangle.model = mTriangleGeom.get();
			mTriangle.transform = AffineTransform::identity();
			mTriangle.transform.position().y() = 10.f;
		}
		return mGfxDriver != nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Player::update() {
		core::Time::get()->update();
		t += core::Time::get()->frameTime();
		if(!mGfxDriver)
			return true;

		glClear(GL_COLOR_BUFFER_BIT);
		
		auto vp = mProjectionMtx;
		auto worldMatrix = Mat44f::identity();

		// For each render obj
		mTriangle.transform.setRotation(math::Quatf(Vec3f(0.f,0.f,1.f), t));
		worldMatrix.block<3,4>(0,0) = mTriangle.transform.matrix();
		auto wvp = vp*worldMatrix;
		glUniformMatrix4fv(0, 1, !Mat44f::is_col_major, wvp.data());
		mTriangle.model->render();

		mGfxDriver->swapBuffers();

		return true;
	}

}	// namespace rev