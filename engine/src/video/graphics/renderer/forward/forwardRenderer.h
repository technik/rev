//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Simple forward renderer
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_FORWARD_FORWARDRENDERER_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_FORWARD_FORWARDRENDERER_H_

#ifdef OPENGL_45
#include <vector>
#include <video/graphics/driver/graphicsDriver.h>
#include <video/graphics/shader/shader.h>
#include <video/basicTypes/texture.h>

namespace rev {
	namespace video {

		class Camera;
		class RendererBackEnd;
		class RenderObj;

		class ForwardRenderer {
		public:
			void init(GraphicsDriver* _driver);
			// TODO: Instead of a camera, this should receive a "render info" struct with
			// information like: use shadows? LOD bias? cam, etc
			// nullptr render target means: use framebuffer
			void render(const std::vector<RenderObj*>& _scene, const Camera& _cam) const;

		private:
			void drawSkyboxCubemap(const Camera& _cam) const;

			Texture*			mSkybox;
			Shader::Ptr			mSkyboxShader;

			GraphicsDriver*		mDriver = nullptr;
			RendererBackEnd*	mBackEnd = nullptr;
			Shader::Ptr			mProgram = nullptr;
		};
	}
}
#endif // OPENGL_45


#endif // _REV_VIDEO_GRAPHICS_RENDERER_FORWARD_FORWARDRENDERER_H_