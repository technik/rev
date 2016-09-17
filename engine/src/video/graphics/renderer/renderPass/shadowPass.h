//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_RENDERPASS_SHADOWPASS_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_RENDERPASS_SHADOWPASS_H_

#include <math/algebra/matrix.h>
#include <math/geometry/types.h>
#include <video/graphics/driver/graphicsDriver.h>
#include <video/graphics/shader/shader.h>
#include <video/graphics/renderer/types/renderTarget.h>

namespace rev {
	namespace video {

		class RenderObj;
		class RendererBackEnd;
		class DebugDrawer;

		class ShadowPass {
		public:
			ShadowPass(GraphicsDriver* _driver, RendererBackEnd* _backEnd, size_t bufferSize);
			~ShadowPass();

			void config(const math::Vec3f& _lightDir, const math::Mat34f& _viewMat, const math::Frustum& _viewFrustum, float _minCaster);
			void finishPass();

			const math::Mat44f& viewProj() const { return mViewProj; }
			Texture* tex() const { return mShadowBuffer->color(); }

			DebugDrawer* mDebug = nullptr;

		private:
			video::Shader::Ptr		mShadowShader = nullptr;
			video::RendererBackEnd*	mBackEnd;
			RenderTarget*			mShadowBuffer;
			GraphicsDriver*			mDriver;
			math::Mat44f			mViewProj;
			RenderTarget*		mMultiSampleShadows;
		};
	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_RENDERPASS_SHADOWPASS_H_