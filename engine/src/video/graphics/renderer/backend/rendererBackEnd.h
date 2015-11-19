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

#include <video/basicTypes/color.h>
#include <video/graphics/shader/shader.h>
#include <math/algebra/matrix.h>

namespace rev {
	namespace video {

		class GraphicsDriver;
		class RenderTarget;

		class RendererBackEnd {
		public:
			struct StaticGeometry {
				bool strip;
				Shader::Ptr shader;
				std::uint16_t nVertices = 0;
				math::Vec3f * vertices = nullptr;
				math::Vec3f * normals = nullptr;
				std::uint16_t nIndices = 0;
				std::uint16_t* indices = nullptr;
				math::Mat34f	transform;
				Color color;
			};

		public:
			RendererBackEnd(GraphicsDriver*);

			void setCamera(const math::Mat34f& _view, const math::Mat44f& _proj);
			void render(const StaticGeometry&);
			void flush();

			RenderTarget*	createRenderTarget();
			void			destroyRenderTarget();

			void			setRenderTarget(RenderTarget* _rt); // If null, set default target

		private:
			void setShader(Shader::Ptr);

		private:
			GraphicsDriver* mDriver;
			math::Mat44f	mViewProj;

			// Render cache
			Shader::Ptr	mCurShader;
			int			mMvpUniform;
			int			mWorldUniform;
			int			mLightDirUniform;
			int			mColorUniform;
		};

	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_BACKEND_BACKENDRENDERER_H_