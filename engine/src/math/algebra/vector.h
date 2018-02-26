//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Generic mathematical vector
#ifndef _REV_MATH_ALGEBRA_VECTOR_H_
#define _REV_MATH_ALGEBRA_VECTOR_H_

#include <cassert>
#include "matrix.h"

namespace rev {
	namespace math {

		template<typename T_, size_t n_>
		struct Vector : public MatrixBase<
			n_,
			1,
			MatrixDenseStorage<
				T_,
				n_,
				1,
				col_major_,
			Vector<T_, n_>
			>>
		{

			template<typename Other_>
			Element	dot(const Other_& _other) const { 
				Element result(0);
				for(size_t i = 0; i < size; ++i)
					result += this->coefficient(i)*_other.coefficient(i);
				return result;
			}

			template<typename Other_>
			Derived cross(const Other_& v) const { 
				Derived result;
				result.x() = y()*v.z() - z()*v.y();
				result.y() = z()*v.x() - x()*v.z();
				result.z() = x()*v.y() - y()*v.x();
				return result;
			}

			Element	norm() const		{ return sqrt(squaredNorm()); }
			Element	squaredNorm() const { return dot(*this); }
		};

		template<typename T_>
		using Vector2 = Vector<T_,2>;
		template<typename T_>
		using Vector3 = Vector<T_,3>;
		template<typename T_>
		using Vector4 = Vector<T_,4>;

		typedef Vector<unsigned, 2> Vec2u;
		typedef Vector<int, 2>		Vec2i;
		typedef Vector<float, 2>	Vec2f;
		typedef Vector<double, 2>	Vec2d;

		typedef Vector3<unsigned>	Vec3u;
		typedef Vector3<int>		Vec3i;
		typedef Vector3<float>		Vec3f;
		typedef Vector3<double>		Vec3d;

		typedef Vector4<unsigned>	Vec4u;
		typedef Vector4<int>		Vec4i;
		typedef Vector4<float>		Vec4f;
		typedef Vector4<double>		Vec4d;
		
	}	// namespace math
}	// namespace rev

#endif // _REV_MATH_ALGEBRA_VECTOR_H_