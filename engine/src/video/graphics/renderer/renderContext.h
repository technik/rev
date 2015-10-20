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

		class RenderObj;

		class RenderContext : public std::set<RenderObj*> {
		public:
			// TODO: Set up lights and everything
		};
	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_RENDERCONTEXT_H_