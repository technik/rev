//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/07
//----------------------------------------------------------------------------------------------------------------------
// Render system's back end, the layer right on top of the video driver
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_BACKEND_RENDERERBACKEND_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_BACKEND_RENDERERBACKEND_H_

#include <cstdint>
#include <math/algebra/vector.h>

#include <video/graphics/shader/shader.h>
#include <math/algebra/matrix.h>

namespace rev {
	namespace video {

		class GraphicsDriver;

		class RendererBackEnd {
		public:
			struct StaticGeometry {
				Shader::Ptr shader;
				std::uint16_t nVertices = 0;
				math::Vec3f * vertices = nullptr;
				std::uint16_t nIndices = 0;
				std::uint16_t* indices = nullptr;
				math::Mat34f	transform;
			};

		public:
			RendererBackEnd(GraphicsDriver*);

			void setCamera(const math::Mat34f& _view, const math::Mat44f& _proj);
			void render(const StaticGeometry&);
			void flush();

		private:
			GraphicsDriver* mDriver;
			math::Mat44f	mViewProj;
		};

	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_BACKEND_BACKENDRENDERER_H_