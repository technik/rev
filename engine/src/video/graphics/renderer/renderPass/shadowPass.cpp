//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "shadowPass.h"

#include <video/graphics/driver/graphicsDriver.h>
#include <video/graphics/renderer/backend/rendererBackEnd.h>
#include <video/graphics/shader/shader.h>

using namespace rev::math;

namespace rev {
	namespace video {

		//--------------------------------------------------------------------------------------------------------------
		ShadowPass::ShadowPass(GraphicsDriver* _driver, RendererBackEnd* _backEnd, size_t _bufferSize)
			: mDriver(_driver)
			, mBackEnd(_backEnd)
		{
			mShadowShader = Shader::manager()->get("shadow");

			mShadowBuffer = mDriver->createRenderTarget({ _bufferSize, _bufferSize}, Texture::EImageFormat::depth, Texture::EByteFormat::eFloat);
		}

		//--------------------------------------------------------------------------------------------------------------
		ShadowPass::~ShadowPass()
		{
		}

		//--------------------------------------------------------------------------------------------------------------
		void ShadowPass::config(const math::Mat34f& _viewMat, const math::Frustum& _viewFrustum, float _depth) {
			Mat33f shadowBasis = Mat33f::identity();
			if (abs(mLightDir * camView.col(1)) < 0.5) { // Do not use the view direction if it is too aligned with the light
				auto lightUp = camView.col(1) ^ mLightDir;
				auto lightSide = mLightDir ^ lightUp;
				lightView.setCol(0, lightSide);
				lightView.setCol(1, lightUp);
				lightView.setCol(2, mLightDir);
			}
			else {
				auto lightUp = camView.col(2) ^ mLightDir;
				auto lightSide = mLightDir ^ lightUp;
				lightView.setCol(0, lightSide);
				lightView.setCol(1, lightUp);
				lightView.setCol(2, mLightDir);
			}
		}
		
		//--------------------------------------------------------------------------------------------------------------
		void ShadowPass::render(RenderObj& _obj) {

		}
	}
}