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
#endif

#include <video/graphics/driver/NativeFrameBuffer.h>
#include <video/graphics/geometry/RenderGeom.h>
#include <video/graphics/geometry/VertexFormat.h>
#include <video/graphics/renderer/backend/rendererBackEnd.h>

#ifdef REV_USE_VULKAN
#include <vulkan/vulkan.h>
#endif // REV_USE_VULKAN

namespace rev {
	namespace video {

		class RenderPass;

		class ForwardRenderer {
		public:
			~ForwardRenderer();
			// Set up the renderer to render into the given frame buffer
			bool init(NativeFrameBuffer&);

			void beginFrame();
			void render(const RenderGeom&, const math::Mat44f& _wvp);
			void endFrame();

		private:
			bool createRenderPass();

			NativeFrameBuffer*	mFrameBuffer;
			RendererBackEnd		mBackEnd;
			RenderPass*			mRenderPass;
		};
	}
}


#endif // _REV_VIDEO_GRAPHICS_RENDERER_FORWARD_FORWARDRENDERER_H_