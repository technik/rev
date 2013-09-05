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
		,mCamLocked(false)
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
	void ForwardRenderer::sortQueue(const Vec3f& _pointOfView, const RenderScene& _scene)
	{
//		const float maxDrawDistance = 100.f;
		mRenderQueue.clear();
		_scene.traverse(_pointOfView, [&,this](const Renderable* _r){
			RenderDesc desc;
			desc.texture = _r->texture();
			mRenderQueue.insert(std::make_pair(desc, _r));
		});
	}

	//------------------------------------------------------------------------------------------------------------------
	void ForwardRenderer::render(const Camera& _pointOfView, const RenderScene& _scene)
	{
		static float angle = 0;
		const float angSpd = 0.1f;
		angle += angSpd * Time::get()->frameTime();
		Mat44f viewProj = _pointOfView.viewProjMatrix();
		Mat34f view;
		_pointOfView.invView().inverse(view);
		Vec3f viewPos = view.col(3);
		if(!mCamLocked)
			mCamLockPos = viewPos;
		mDriver->setZCompare(true);
		math::Vec3f lightDir(cosf(angle), sinf(angle), -2.f);

		RenderDesc currentDesc;
		currentDesc.texture = nullptr;
		mDriver->setUniform(mDiffTextUniform, mXorText);
		sortQueue(mCamLockPos, _scene);
		for(auto entry : mRenderQueue) {
			const Renderable* renderable = entry.second;
			if(!renderable->isVisible)
				continue;
			Mat34f invModel;
			renderable->m.inverse(invModel);
			mDriver->setUniform(mMvpUniform, viewProj * renderable->m);
			mDriver->setUniform(mLightUniform, invModel.rotate(lightDir));
			mDriver->setUniform(mViewPosUniform, invModel * viewPos);
			if(!(entry.first == currentDesc)) {
				currentDesc = entry.first;
				const Image* texture = renderable->texture();
				if(!texture)
					texture = mXorText;
				mDriver->setUniform(mDiffTextUniform, texture);
			}
			renderable->render();
		}
	}

}	// namespace graphics3d
}	// namespace rev
