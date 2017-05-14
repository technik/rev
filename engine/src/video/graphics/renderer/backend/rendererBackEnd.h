//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/07
//----------------------------------------------------------------------------------------------------------------------
// Render system's back end, the layer right on top of the video driver
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_BACKEND_RENDERERBACKEND_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_BACKEND_RENDERERBACKEND_H_

#include <cstdint>


#include <math/algebra/matrix.h>
#include <math/algebra/vector.h>
#include <video/graphics/driver/graphicsDriver.h>
#include <video/graphics/shader/shader.h>

//#include <video/basicTypes/color.h>

namespace rev {
	namespace video {

		struct StaticRenderMesh;

		class RendererBackEnd {
		public:
			struct DrawInfo {
				Shader::Ptr		program;
				math::Vec3f		lightDir;
				math::Vec3f		lightClr;
				math::Vec3f		viewPos;
				math::Mat44f	wvp; // Model view projection
			};

			struct DrawCall {
				/// Things line uniforms, and render config
				DrawInfo			renderStateInfo;
				StaticRenderMesh*	mesh;
			};

			RendererBackEnd(GraphicsDriver* _driver) : mDriver(_driver) {}
			void draw(const DrawCall&);
			void flush();

		private:
			GraphicsDriver* mDriver = nullptr;
		};

	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_BACKEND_BACKENDRENDERER_H_