//----------------------------------------------------------------------------------------------------------------------
// Graphics 3d module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/10
//----------------------------------------------------------------------------------------------------------------------
// basic forward renderer

#include "forwardRenderer.h"

#include <cmath>
#include <revCore/time/time.h>
#include <revGraphics3d/camera/camera.h>
#include <revGraphics3d/renderable/renderable.h>
#include <revGraphics3d/renderable/renderScene.h>
#include <revVideo/driver3d/driver3d.h>
#include <revVideo/types/image/image.h>
#include <revVideo/videoDriver/videoDriver.h>

using namespace rev::math;
using namespace rev::video;

namespace rev { namespace graphics3d {

	//------------------------------------------------------------------------------------------------------------------
	ForwardRenderer::ForwardRenderer()
		:mDriver(video::VideoDriver::getDriver3d())
	{
		// Shader
		mMvpUniform = mDriver->getUniformLocation("mvp");
		mLightUniform = mDriver->getUniformLocation("lightDir");
		mViewPosUniform = mDriver->getUniformLocation("viewPos");
		mDiffTextUniform = mDriver->getUniformLocation("diffTex");

		// Create XOR Texture
		uint8_t* xorBuffer = new uint8_t[4* 256 * 256];
		for(unsigned i = 0; i < 256; ++i)
			for(unsigned j = 0; j < 256; ++j) {
				uint8_t intensity = uint8_t(i ^ j);
				xorBuffer[4*(j+256*i)+0] = intensity;
				xorBuffer[4*(j+256*i)+1] = intensity;
				xorBuffer[4*(j+256*i)+2] = intensity;
				xorBuffer[4*(j+256*i)+3] = 255;
			}
		mXorText = new Image(xorBuffer, 256, 256);
	}

	//------------------------------------------------------------------------------------------------------------------
	void ForwardRenderer::render(const Camera& _pointOfView, const RenderScene& _scene)
	{
		static float angle = 0;
		const float angSpd = 0.01f;
		angle += angSpd * Time::get()->frameTime();
		Mat44f viewProj = _pointOfView.viewProjMatrix();
		Mat34f view;
		_pointOfView.invView().inverse(view);
		Vec3f viewPos(view[0][3], view[1][3], view[2][3]);
		mDriver->setZCompare(true);
		math::Vec3f lightDir(cosf(angle), sinf(angle), -2.f);
		unsigned nRendered = 0;
		_scene.traverse([&,this](const Renderable* _renderable)
		{
			if(0 != mMaxRenderables) {
				if(nRendered == mMaxRenderables)
					return;
				else ++nRendered;
			}
			Mat34f invModel;
			_renderable->m.inverse(invModel);
			mDriver->setUniform(mMvpUniform, viewProj * _renderable->m);
			mDriver->setUniform(mLightUniform, invModel.rotate(lightDir));
			mDriver->setUniform(mViewPosUniform, invModel * viewPos);
			const Image* texture = _renderable->texture();
			if(!texture)
				texture = mXorText;
			mDriver->setUniform(mDiffTextUniform, texture);
			_renderable->render();
		});
	}

}	// namespace graphics3d
}	// namespace rev
