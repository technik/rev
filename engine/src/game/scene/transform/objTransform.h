//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include "../component.h"
#include "../sceneNode.h"
#include <math/algebra/matrix.h>
#include <math/algebra/quaternion.h>
#include <math/algebra/vector.h>
#include <cjson/json.h>

namespace rev {
	namespace game {

		class ObjTransform : public Component {
		public:
			// Construction
			ObjTransform(SceneNode& _owner) : Component(_owner) {}

			// Accessors
					void			setPosition	(const math::Vec3f& _p)	{ mMatrix.setCol(3,_p); }
					math::Vec3f		position	() const				{ return mMatrix.col(3); }

					void			setRotation	(const math::Quatf& _q) { mMatrix = math::Mat34f(_q, position()); }
					void			rotate		(const math::Quatf& _q);

					math::Mat34f&	matrix	()						{ return mMatrix; }
			const	math::Mat34f&	matrix	() const				{ return mMatrix; }
			const	math::Mat34f&	worldMatrix	() const			{ return mWorldMtx; }

			static ObjTransform* construct(const cjson::Json& _data, SceneNode& _owner) {
				ObjTransform* t = new ObjTransform(_owner);
				if (_data.contains("mat")) {
					const cjson::Json& mat = _data["mat"];
					for(auto i = 0; i < 4; ++i) {
						math::Vec3f col = math::Vec3f(mat(i + 0), mat(i + 4), mat(i + 8));
						t->mMatrix.setCol(i, col);
					}
				}
				else if (_data.contains("pos")) {
					const cjson::Json& pos = _data["pos"];
					t->mMatrix = math::Mat34f::identity();
					math::Vec3f col = math::Vec3f(pos(0), pos(1), pos(2));
					t->mMatrix.setCol(3, col);
				}
				return t;
			}

		private:
			void ObjTransform::update() {
				ObjTransform* parentTransform = nullptr;
				if(node().parent())
					parentTransform = node().parent()->getComponent<ObjTransform>();
				if(parentTransform)
					mWorldMtx = parentTransform->worldMatrix() * mMatrix;
					
			}

			math::Mat34f mMatrix = math::Mat34f::identity();
			math::Mat34f mWorldMtx = math::Mat34f::identity();
		};

		//-------------------------------------------------------------------------------------------
		inline void ObjTransform::rotate(const math::Quatf& _q) {
			math::Vec3f pos = position();
			mMatrix = math::Mat34f(_q) * mMatrix;
			setPosition(pos);
		}

} }	// namespace rev::game
