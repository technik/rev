//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "shadowPass.h"

#include <video/graphics/driver/graphicsDriver.h>
#include <video/graphics/renderer/backend/rendererBackEnd.h>
#include <video/graphics/shader/shader.h>
#include <video/graphics/renderer/types/renderTarget.h>
#include <video/graphics/renderer/debugDrawer.h>
#include <video/graphics/renderer/renderContext.h>
#include <video/basicTypes/camera.h>

using namespace rev::math;

namespace rev {
	namespace video {

		//--------------------------------------------------------------------------------------------------------------
		ShadowPass::ShadowPass(GraphicsDriver* _driver, RendererBackEnd* _backEnd, size_t _bufferSize)
			: mDriver(_driver)
			, mBackEnd(_backEnd)
		{
			mShadowShader = Shader::manager()->get("shadow");

			mShadowBuffer = new RenderTarget();
			Vec2u size(_bufferSize);
			Texture* depthTex = new Texture(size, Texture::InternalFormat::depth, false);
			mShadowBuffer->setTargetDepth(depthTex);
			Texture* shadowMap = new Texture(size, Texture::InternalFormat::rg32f, false);
			shadowMap->enableMipMaps();
			mShadowBuffer->setTargetColor(shadowMap);

			Texture* msClr = new Texture(size, Texture::InternalFormat::rg32f, true);
			Texture* msDpt = new Texture(size, Texture::InternalFormat::depth, true);
			mMultiSampleShadows = new RenderTarget();
			mMultiSampleShadows->setTargetColor(msClr);
			mMultiSampleShadows->setTargetDepth(msDpt);
		}

		//--------------------------------------------------------------------------------------------------------------
		ShadowPass::~ShadowPass()
		{
			delete mShadowBuffer->depth();
			delete mShadowBuffer->color();
			delete[] mShadowBuffer;
		}

		//--------------------------------------------------------------------------------------------------------------
		void ShadowPass::config(const math::Vec3f& _lightDir, const math::Mat34f& _viewMat, const math::Frustum& _viewFrustum, float _minCaster) {
			// ---- Compute shadow transform axes ----
			Mat34f shadowBasis = Mat34f::identity();
			if (abs(_lightDir * _viewMat.col(1)) < 0.71f) { // Only use the view direction if it is not too aligned with the light
				auto shadowX = (_viewMat.col(1) ^ _lightDir).normalized(); // view.y ^ light;
				auto shadowY = _lightDir ^ shadowX;
				shadowBasis.setCol(0, shadowX);
				shadowBasis.setCol(1, shadowY);
			}
			else {
				auto shadowY = (_lightDir ^ _viewMat.col(0)).normalized();
				auto shadowX = shadowY ^_lightDir;
				shadowBasis.setCol(0, shadowX);
				shadowBasis.setCol(1, shadowY);
			}
			shadowBasis.setCol(2, _lightDir);

			// ---- Project frustum into the basis to have its vertices in world space ---
			Vec3f frustumWorld[8];
			for (size_t i = 0; i < 8; ++i) {
				frustumWorld[i] = _viewMat*_viewFrustum.vertices()[i];
			}
			// Transfrom frustum into shadow space to compute bounding box
			Mat34f invShadow;
			shadowBasis.inverse(invShadow);
			Vec3f shadowPoint0 = invShadow * frustumWorld[0];
			Vec3f shadowPoint1 = invShadow * frustumWorld[1];
			BBox shadowBB(shadowPoint0, shadowPoint1);
			for (size_t i = 2; i < 8; ++i) {
				shadowBB.merge(invShadow * frustumWorld[i]);
			}
			shadowBB.merge(Vec3f(0.f,0.f,-_minCaster));
			// Recenter shadow basis
			shadowBasis.setCol(3, shadowBasis * shadowBB.center());
			shadowBasis.inverse(invShadow);
			Mat44f shadowProj = Mat44f::ortho(shadowBB.size());

			mBackEnd->setCamera(shadowBasis, shadowProj);
			mViewProj = shadowProj * invShadow;
			// Configure renderer
			//mDriver->setRenderTarget(mShadowBuffer);
			mDriver->setRenderTarget(mMultiSampleShadows);

			mDriver->setViewport(math::Vec2i(0, 0), mShadowBuffer->depth()->size());
			mDriver->clearZBuffer();
			mDriver->setClearColor(Color(1.0));
			mDriver->clearColorBuffer();
			mDriver->setCulling(GraphicsDriver::ECulling::eFront);
			mBackEnd->setShader(mShadowShader);

			if (mDebug) {
				mDebug->drawBasis(shadowBasis);
				Vec3f centroidWorld = _viewMat * Vec3f(0.f, _viewFrustum.centroid(), 0.f);
				mDebug->drawLine(centroidWorld-_lightDir*50.f, centroidWorld, Color(1.f,0.f,0.f));
				mDebug->drawLine(centroidWorld, centroidWorld + _lightDir*50.f, Color(0.f, 1.f, 1.f));
				mDebug->drawFrustum(shadowBasis, Frustum(shadowBB.size()), Color(0.5f,0.5f,0.f));
			}


		}

		//--------------------------------------------------------------------------------------------------------------
		void ShadowPass::finishPass() {
			glFinish();
			Vec2u size = mShadowBuffer->color()->size();
			glBindFramebuffer(GL_READ_FRAMEBUFFER, mMultiSampleShadows->glId());
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mShadowBuffer->glId());   // Make sure no FBO is set as the draw framebuffer
			glBlitFramebuffer(0,0, (GLint)size.x, (GLint)size.y,0,0, (GLint)size.x, (GLint)size.y, GL_COLOR_BUFFER_BIT, GL_LINEAR);
			mShadowBuffer->color()->generateMipMaps();
		}
	}
}