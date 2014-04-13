//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/13
//----------------------------------------------------------------------------------------------------------------------
// Simple forward renderer
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_FORWARD_FORWARDRENDERER_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_FORWARD_FORWARDRENDERER_H_

#include <video/graphics/renderer/backend/rendererBackEnd.h>
#include <video/graphics/shader/shader.h>

namespace rev {
	namespace video {

		class GraphicsDriver;
		class RenderObj;
		class Window;

		class ForwardRenderer {
		public:
			ForwardRenderer(Window*);
			~ForwardRenderer();

			void startFrame();
			void finishFrame();
			void renderObject(const RenderObj&);

		private:
			video::Shader::Ptr		mShader = nullptr;
			video::RendererBackEnd	mBackEnd;
			GraphicsDriver*			mGraphicsDriver;
		};
	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_FORWARD_FORWARDRENDERER_H_