//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Base renderable object
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_RENDEROBJ_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_RENDEROBJ_H_

#include<core/components/component.h>
#include<core/components/sceneNode.h>
#include<core/context/contextObj.h>
#include<math/algebra/matrix.h>
#include<video/graphics/renderer/renderContext.h>

namespace rev {
	namespace video {

		class Material;
		class RenderMesh;

		class RenderObj : public core::ContextObj<RenderObj, RenderContext>, public core::Component {
		public:
			RenderObj(RenderMesh* _mesh) : mMesh(_mesh) {}
			
			RenderMesh*			mesh		() const { return mMesh; }
			const math::Mat34f&	transform	() const { return node()->transform(); }

			Material* mMaterial = nullptr;

			struct BoundingBox {
				math::Vec3f min, max;
			} mBBox;
		private:
			RenderMesh* mMesh;

		};
	}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_GRAPHICS_RENDERER_RENDEROBJ_H_