//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Base renderable object
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_RENDEROBJ_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_RENDEROBJ_H_

#include <core/components/component.h>
#include <core/components/sceneNode.h>
#include <math/algebra/matrix.h>
#include <math/geometry/types.h>
#include <video/graphics/staticRenderMesh.h>

namespace rev {
	namespace video {

		class Material;

		class RenderObj : public core::Component {
		public:
			RenderObj(StaticRenderMesh* _mesh) : mMesh(_mesh) {
			}

			StaticRenderMesh*	mesh() const { return mMesh; }
			const math::Mat34f&	transform() const { return node()->transform(); }

			//Material* mMaterial = nullptr;
		private:
			StaticRenderMesh* mMesh;

		};
	}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_GRAPHICS_RENDERER_RENDEROBJ_H_