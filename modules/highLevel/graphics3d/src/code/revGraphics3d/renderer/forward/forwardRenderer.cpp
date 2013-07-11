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
#include <revVideo/types/shader/pixel/pxlShader.h>
#include <revVideo/types/shader/shader.h>
#include <revVideo/types/shader/vertex/vtxShader.h>
#include <revVideo/types/shader/vertex/openGL21/vtxShaderOpenGL21.h>
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
		VtxShader* vtxShader = VtxShader::get("test.vtx");
		// ----- WARNING: Highly platform dependent code -----
		VtxShaderOpenGL21* vtx21 = static_cast<VtxShaderOpenGL21*>(vtxShader);
		vtx21->addAttribute("vertex");
		vtx21->addAttribute("normal");
		// ----- End of platform dependent code --------------
		mBasicShader = Shader::get(std::make_pair(vtxShader, PxlShader::get("test.pxl")));
		mMvpUniform = mBasicShader->getUniformLocation("mvp");
		mLightUniform = mBasicShader->getUniformLocation("lightDir");
		//mColorUniformId = mShader->getUniformLocation("color");
	}

	//------------------------------------------------------------------------------------------------------------------
	void ForwardRenderer::render(const Camera& _pointOfView, const RenderScene& _scene)
	{
		Mat44f viewProj = _pointOfView.viewProjMatrix();
		mDriver->setZCompare(true);
		mDriver->setShader(mBasicShader);
		mDriver->setUniform(mLightUniform, math::Vec3f(1.f, 1.f, -2.f));
		_scene.traverse([&,this](const Renderable* _renderable)
		{
			mDriver->setUniform(mMvpUniform, viewProj * _renderable->m);
			_renderable->render();
		});
	}

}	// namespace graphics3d
}	// namespace rev
