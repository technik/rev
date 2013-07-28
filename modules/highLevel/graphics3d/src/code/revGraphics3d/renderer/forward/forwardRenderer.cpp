//----------------------------------------------------------------------------------------------------------------------
// Graphics 3d module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/10
//----------------------------------------------------------------------------------------------------------------------
// basic forward renderer

#include "forwardRenderer.h"

#include <revGraphics3d/camera/camera.h>
#include <revGraphics3d/renderable/renderable.h>
#include <revGraphics3d/renderable/renderScene.h>
#include <revVideo/driver3d/driver3d.h>
#include <revVideo/videoDriver/videoDriver.h>

using rev::math::Mat34f;
using rev::math::Mat44f;
using namespace rev::video;

namespace rev { namespace graphics3d {

	//------------------------------------------------------------------------------------------------------------------
	ForwardRenderer::ForwardRenderer()
		:mDriver(video::VideoDriver::getDriver3d())
	{
		// Shaders
		mMvpUniform = mDriver->getUniformLocation("mvp");
		mLightUniform = mDriver->getUniformLocation("lightDir");
		//mColorUniformId = mShader->getUniformLocation("color");
	}

	//------------------------------------------------------------------------------------------------------------------
	void ForwardRenderer::render(const Camera& _pointOfView, const RenderScene& _scene)
	{
		Mat44f viewProj = _pointOfView.viewProjMatrix();
		mDriver->setZCompare(true);
		_scene.traverse([&,this](const Renderable* _renderable)
		{
			Mat34f invModel;
			_renderable->m.inverse(invModel);
			mDriver->setUniform(mMvpUniform, viewProj * _renderable->m);
			mDriver->setUniform(mLightUniform, invModel.rotate(math::Vec3f(1.f, 0.f, -2.f)));
			_renderable->render();
		});
	}

}	// namespace graphics3d
}	// namespace rev
