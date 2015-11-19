//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Simple deferred renderer
#ifndef _REV_VIDEO_GRAPHICS_DRIVER_OPENGL_RENDERTARGETGL_H_
#define _REV_VIDEO_GRAPHICS_DRIVER_OPENGL_RENDERTARGETGL_H_

#include <video/graphics/renderer/types/renderTarget.h>

namespace rev {
	namespace video {

		class RenderTargetGL : public RenderTarget {
		public:
			unsigned id;
			unsigned textureId;
		};
	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_TYPES_RENDERTARGET_H_

