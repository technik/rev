//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Simple deferred renderer
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_TYPES_RENDERTARGET_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_TYPES_RENDERTARGET_H_

namespace rev {
	namespace video {

		class Texture;

		class RenderTarget {
		public:
			Texture* tex;
		};
	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_TYPES_RENDERTARGET_H_
