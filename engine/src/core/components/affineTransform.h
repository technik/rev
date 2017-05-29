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
			const	math::Vec3f&	position	() const	{ return matrix.col(3); }

					math::Mat34f&	transform	()			{ return matrix; }
			const	math::Mat34f&	transform	() const	{ return matrix; }

			static AffineTransform* construct(const cjson::Json& _data) {
				AffineTransform* t = new AffineTransform;
				if (_data.contains("mat")) {
					const cjson::Json& mat = _data["mat"];
					for(auto i = 0; i < 4; ++i) {
						math::Vec3f col = math::Vec3f(mat(i + 0), mat(i + 4), mat(i + 8));
						t->matrix.setCol(i, col);
					}
				}
				else if (_data.contains("pos")) {
					const cjson::Json& pos = _data["pos"];
					t->matrix = math::Mat34f::identity();
					math::Vec3f col = math::Vec3f(pos(0), pos(1), pos(2));
					t->matrix.setCol(3, col);
				}
				return t;
			}

		private:
			math::Mat34f matrix;

		};

} }	// namespace rev::core
