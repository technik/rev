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
#include <video/graphics/renderer/renderMesh.h>

namespace rev {
	namespace video {

		class Material;

		class RenderObj : public core::Component {
		public:
			RenderObj(RenderMesh* _mesh) : mMesh(_mesh) {
				mBBox.min = _mesh->bbMin;
				mBBox.max = _mesh->bbMax;
			}

			RenderMesh*			mesh() const { return mMesh; }
			const math::Mat34f&	transform() const { return node()->transform(); }

			Material* mMaterial = nullptr;

			bool castShadows = true;
			bool recvShadows = true;

			math::BBox mBBox;
		private:
			RenderMesh* mMesh;

		};
	}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_GRAPHICS_RENDERER_RENDEROBJ_H_