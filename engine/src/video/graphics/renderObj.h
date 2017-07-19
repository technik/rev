//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Base renderable object
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_RENDEROBJ_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_RENDEROBJ_H_
#ifndef REV_USE_VULKAN

#include <core/components/component.h>
#include <core/components/sceneNode.h>
#include <math/algebra/matrix.h>
#include <math/geometry/types.h>
#include <video/graphics/staticRenderMesh.h>
#include <video/graphics/material.h>

namespace rev {
	namespace video {

		class RenderObj : public core::Component {
		public:
			RenderObj(StaticRenderMesh* _mesh) : mMesh(_mesh) {
			}

			StaticRenderMesh*	mesh() const { return mMesh; }
			//const math::Mat34f&	transform() const { return node()->component<Transform>()->worldMatrix(); }

			Material* material = nullptr;

			static RenderObj* construct(const cjson::Json& _data) {
				std::string fileName = _data["file"];
				RenderObj* obj = new RenderObj(StaticRenderMesh::loadFromFile(fileName));
				if (_data.contains("material"))
				{
					std::string materialName = _data["material"];
					obj->material = Material::loadFromFile(materialName);
				}
				return obj;
			}
		private:
			StaticRenderMesh* mMesh;
		};
	}	// namespace video
}	// namespace rev

#endif // !REV_USE_VULKAN
#endif // _REV_VIDEO_GRAPHICS_RENDERER_RENDEROBJ_H_