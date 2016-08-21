//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_RENDERPASS_WHITEPASS_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_RENDERPASS_WHITEPASS_H_

#include <math/algebra/matrix.h>
#include <math/geometry/types.h>
#include <video/graphics/shader/shader.h>
#include <video/graphics/renderer/types/renderTarget.h>

namespace rev {
	namespace video {

		class GraphicsDriver;
		class RenderObj;
		class RendererBackEnd;
		class DebugDrawer;

		class WhitePass {
		public:
			WhitePass(GraphicsDriver* _driver, RendererBackEnd* _backEnd, const math::Vec2u& _bufferSize);
			~WhitePass();

			void config(const math::Mat34f& _viewMat, const math::Frustum& _viewFrustum, const math::Mat44f& _shadowVp);

			Texture* tex() const { return mShadowBuffer->tex; }

			DebugDrawer* mDebug = nullptr;

		private:
			video::Shader::Ptr		mShadowShader = nullptr;
			video::RendererBackEnd*	mBackEnd;
			RenderTarget*			mShadowBuffer;
			GraphicsDriver*			mDriver;
			math::Mat44f			mViewProj;
		};
	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_RENDERPASS_WHITEPASS_H_