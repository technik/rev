//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include "component.h"
#include <math/algebra/matrix.h>
#include <math/algebra/quaternion.h>
#include <math/algebra/vector.h>
#include <cjson/json.h>

namespace rev {
	namespace core {

		class AffineTransform : public Component {
		public:
					void			setPosition	(const math::Vec3f& _p)	{ mMatrix.setCol(3,_p); }
					math::Vec3f		position	() const				{ return mMatrix.col(3); }

					void			setRotation	(const math::Quatf& _q) { mMatrix = math::Mat34f(_q, position()); }
					void			rotate		(const math::Quatf& _q);

					math::Mat34f&	matrix	()						{ return mMatrix; }
			const	math::Mat34f&	matrix	() const				{ return mMatrix; }

			static AffineTransform* construct(const cjson::Json& _data) {
				AffineTransform* t = new AffineTransform;
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
			math::Mat34f mMatrix;

		};

		//-------------------------------------------------------------------------------------------
		inline void AffineTransform::rotate(const math::Quatf& _q) {
			math::Vec3f pos = position();
			mMatrix = math::Mat34f(_q) * mMatrix;
			setPosition(pos);
		}

} }	// namespace rev::core
