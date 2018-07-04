//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2018 Carmelo J Fdez-Aguera
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#include "ForwardRenderer.h"
#include "ForwardPass.h"
#include "graphics/driver/renderTarget.h"
#include "graphics/scene/renderScene.h"

namespace rev { namespace graphics {

	//------------------------------------------------------------------------------------------------------------------
	void ForwardRenderer::init(GraphicsDriverGL& driver, RenderTarget& _renderTarget)
	{
		mRenderTarget = &_renderTarget;
		mBackEnd = std::make_unique<BackEndRenderer>(driver);
		mForwardPass = std::make_unique<ForwardPass>(*mBackEnd, driver);
		mShadowPass = std::make_unique<ShadowMapPass>(*mBackEnd, driver, math::Vec2u(1024, 1024));
	}

	//------------------------------------------------------------------------------------------------------------------
	void ForwardRenderer::render(const RenderScene& scene) {
		mBackEnd->beginFrame();

		if(!mRenderTarget)
			return;

		mShadowPass->render(scene);
		//mForwardPass->render(eye,scene,*mRenderTarget,mShadowPass.get());
		mForwardPass->render(scene,*mRenderTarget,nullptr);
	}

}}
