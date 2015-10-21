//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Render context. Includes a list of objects
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_RENDERCONTEXT_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_RENDERCONTEXT_H_

#include <set>

namespace rev {
	namespace video {

		class Camera;
		class RenderObj;

		class RenderContext : public std::set<RenderObj*> {
		public:
			void setCamera(const Camera* _cam) { mCamera = _cam; }
			const Camera* camera() const { return mCamera; }

		private:
			const Camera* mCamera;
		};
	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_RENDERCONTEXT_H_