//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "shadowPass.h"

#include <video/graphics/driver/graphicsDriver.h>
#include <video/graphics/renderer/backend/rendererBackEnd.h>
#include <video/graphics/shader/shader.h>
#include <video/graphics/renderer/types/renderTarget.h>

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
		void ShadowPass::config(const math::Vec3f& _lightDir, const math::Mat34f& _viewMat, const math::Frustum& _viewFrustum, float _depth) {
			// ---- Compute shadow transform axes ----
			Mat34f shadowBasis = Mat34f::identity();
			if (abs(_lightDir * _viewMat.col(1)) < 0.71f) { // Only use the view direction if it is not too aligned with the light
				auto shadowX = _lightDir ^ _viewMat.col(1); // light ^ view.y;
				auto shadowY = _lightDir ^ shadowX;
				shadowBasis.setCol(0, shadowX);
				shadowBasis.setCol(1, shadowY);
				// auto lightUp = _viewMat.col(1) ^ _lightDir;
				// auto lightSide = _lightDir ^ lightUp;
				// shadowBasis.setCol(0, lightSide);
				// shadowBasis.setCol(1, lightUp);
				// shadowBasis.setCol(2, _lightDir);
			}
			else {
				auto shadowY = _lightDir ^ _viewMat.col(0);
				auto shadowX = _lightDir ^ shadowY;
				shadowBasis.setCol(0, shadowX);
				shadowBasis.setCol(1, shadowY);
				// auto lightUp = _viewMat.col(2) ^ _lightDir;
				// auto lightSide = _lightDir ^ lightUp;
				// shadowBasis.setCol(0, lightSide);
				// shadowBasis.setCol(1, lightUp);
				// shadowBasis.setCol(2, _lightDir);
			}
			shadowBasis.setCol(2, _lightDir);

			// ---- Project frustum into the basis to compute the bounding box ---
			Vec3f frustumWorld[8];
			for (size_t i = 0; i < 8; ++i) {
				frustumWorld[i] = _viewMat*_viewFrustum.vertices()[i];
			}
			Vec3f centroidWorld = _viewMat * Vec3f(0.f, _viewFrustum.centroid(), 0.f);
			BBox shadowBB(centroidWorld, centroidWorld);
			// Transfrom frustum into shadow space to compute bounding box
			Mat34f invShadow;
			shadowBasis.inverse(invShadow);
			for (size_t i = 0; i < 8; ++i) {
				shadowBB.merge(invShadow * frustumWorld[i]);
			}
			// Recenter shadow basis
			shadowBasis.setCol(3, shadowBasis*shadowBB.center()); // TODO: Match lower surface of the bb with lower side of the frustum
			Mat44f shadowProj = Mat44f::ortho({ shadowBB.size().x,shadowBB.size().y, _depth });

			// Configure renderer
			mDriver->setRenderTarget(mShadowBuffer);
			mDriver->setViewport(math::Vec2i(0, 0), mShadowBuffer->tex->size);
			mDriver->clearZBuffer();
			mDriver->setCulling(GraphicsDriver::ECulling::eFront);
			mBackEnd->setShader(mShadowShader);
		}
	}
}