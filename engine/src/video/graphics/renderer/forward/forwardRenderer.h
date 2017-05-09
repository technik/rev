//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/13
//----------------------------------------------------------------------------------------------------------------------
// Simple forward renderer
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_FORWARD_FORWARDRENDERER_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_FORWARD_FORWARDRENDERER_H_


namespace rev {
	namespace video {

		class RenderScene;
		class GraphicsDriver;
		class RendererBackEnd;

		class ForwardRenderer {
		public:
			void init(GraphicsDriver* _driver);
			void render(const RenderScene& _scene);

		private:
			RendererBackEnd*	mBackEnd;
		};
	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_FORWARD_FORWARDRENDERER_H_