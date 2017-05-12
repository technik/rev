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
#include <video/graphics/driver/graphicsDriver.h>

//#include <video/basicTypes/color.h>
//#include <video/graphics/shader/shader.h>
//#include <math/algebra/matrix.h>

namespace rev {
	namespace video {

		struct StaticRenderMesh;

		class RendererBackEnd {
		public:
			struct DrawInfo {
				GLuint			program;
				math::Vec3f		lightDir;
				math::Vec3f		lightClr;
			};

			struct DrawCall {
				/// Things line uniforms, and render config
				DrawInfo			renderStateInfo;
				StaticRenderMesh*	mesh;
			};

			RendererBackEnd(GraphicsDriver* _driver) : mDriver(_driver) {}
			void draw(const DrawCall&);

		private:
			GraphicsDriver* mDriver = nullptr;
		}
		/*
		class RendererBackEnd {
		public:
			struct StaticGeometry {
				bool strip;
				std::uint16_t nVertices = 0;
				math::Vec3f * vertices = nullptr;
				math::Vec3f * normals = nullptr;
				math::Vec2f * uvs = nullptr;
				std::uint16_t nIndices = 0;
				std::uint16_t* indices = nullptr;
				math::Mat34f	transform;
				Color color;
			};

		public:
			RendererBackEnd(GraphicsDriver*);

			void setView(const math::Mat34f& _view);
			void setCamera(const math::Mat34f& _view, const math::Mat44f& _proj);
			void setShadowVp(size_t i, const math::Mat44f& _svp) { mSvp[i] = _svp; }
			void render(const StaticGeometry&);
			void flush();

			void setShader(Shader::Ptr);

		private:
			GraphicsDriver* mDriver;
			math::Mat44f	mViewProj;
			math::Mat44f	mSvp[3];
			math::Mat34f	mInvView;

			// Render cache
			Shader::Ptr	mCurShader;
			int			mMvpUniform;
			int			mShadowMvpUniform[3];
			int			mWorldRotUniform;
			int			mLightDirUniform;
			int			mColorUniform;
		};

	}
}

*/

#endif // _REV_VIDEO_GRAPHICS_RENDERER_BACKEND_BACKENDRENDERER_H_