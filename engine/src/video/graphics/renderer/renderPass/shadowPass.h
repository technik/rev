//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_RENDERPASS_SHADOWPASS_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_RENDERPASS_SHADOWPASS_H_

#include <math/algebra/matrix.h>
#include <math/geometry/types.h>

namespace rev {
	namespace video {

		class GraphicsDriver;
		class RenderObj;
		class RendererBackEnd;

		class ShadowPass {
		public:
			ShadowPass(GraphicsDriver* _driver, RendererBackEnd* _backEnd, size_t bufferSize);
			~ShadowPass();

			void config(const math::Mat34f& _viewMat, const math::Frustum& _viewFrustum, float _depth);
			void render(RenderObj& _obj);

		private:
			video::Shader::Ptr		mShadowShader = nullptr;
			video::RendererBackEnd*	mBackEnd;
			RenderTarget*			mShadowBuffer;
			GraphicsDriver*			mDriver;
		};
	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_RENDERPASS_SHADOWPASS_H_