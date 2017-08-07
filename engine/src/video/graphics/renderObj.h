//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Renderable object
#pragma once

#include <math/algebra/matrix.h>
#include <game/scene/transform/objTransform.h>
#include <video/graphics/geometry/RenderGeom.h>

namespace rev {
	namespace video {

		class RenderObj {
		public:
			RenderObj(RenderGeom* _mesh, const game::ObjTransform& _transform) : mMesh(_mesh), mTransform(_transform) {
			}

			RenderGeom*	mesh() const { return mMesh; }
			
			math::Mat34f	transform() const { return mTransform.worldMatrix(); }

			//Material* material = nullptr;

			static RenderObj* construct(const cjson::Json& _data, game::SceneNode& _owner) {
				std::string fileName = _data["file"];
				RenderObj* obj = new RenderObj(RenderGeom::loadFromFile(fileName), *_owner.getComponent<game::ObjTransform>());
				/*if (_data.contains("material"))
				{
					std::string materialName = _data["material"];
					obj->material = Material::loadFromFile(materialName);
				}*/
				return obj;
			}
		private:

			RenderGeom* mMesh;
			const game::ObjTransform& mTransform;
		};
	}	// namespace video
}	// namespace rev