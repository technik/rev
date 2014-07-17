//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Generic mathematical vector
#ifndef _REV_MATH_ALGEBRA_VECTOR_H_
#define _REV_MATH_ALGEBRA_VECTOR_H_

#include <cassert>

namespace rev {
	namespace math {

		// A vector of dimension _n
		template<typename T_, unsigned n_>
		class Vector {
		public:
			/// 666 TODO: Implemente generic dimension vector
			inline T_&			operator[](unsigned _n)			{ return m[_n]; }
			inline const T_&	operator[](unsigned _n) const	{ return m[_n]; }
		private:
			T_ m[n_];
		};

		// 2D vector
		template<typename T_>
		class Vector<T_, 2> {
		public:
			T_ x, y;
		public:
			// Construction
			Vector() = default;
			Vector(const Vector&) = default;
			Vector(const T_& _x, const T_& _y) : x(_x), y(_y) {}

			// Comparison
			bool operator==(const Vector&) const;
			bool operator!=(const Vector&) const;

			// Adition and substraction
			Vector	operator+	(const Vector&) const;	// Adition
			Vector&	operator+=	(const Vector&);		// Adition
			Vector	operator-	(const Vector&) const;	// Substraction
			Vector&	operator-=	(const Vector&);		// Substraction
			Vector	operator-	() const;				// Return the oposite of this vector

			// Products
			Vector	operator*	(const T_) const;		// Product by scalar
			Vector&	operator*=	(const T_);				// Product by scalar
			Vector	operator/	(const T_) const;		// Division by scalar
			Vector&	operator/=	(const T_);				// Division by scalar
			T_		operator*	(const Vector&) const;	// Dot product
			T_		operator^	(const Vector&) const;	// Cross product

			// Vector norm and normalization
			T_		norm() const;						//< Vector norm
			T_		sqNorm() const;						///< Vector square norm
			Vector	normalized() const;					///< Returns a normalized version of the vector
			Vector&	normalize();						///< Normalizes the vector
		};

		// 3D vector
		template<typename T_>
		class Vector<T_, 3> {
		public:
			T_ x, y, z;
		public:
			// Construction
			Vector() = default;
			Vector(const Vector&) = default;
			Vector(const Vector<T_,2>& _v, T_ _z) :x(_v.x), y(_v.y), z(_z) {}
			explicit Vector(const T_ _v) :x(_v), y(_v), z(_v) {}
			Vector(T_ _x, T_ _y, T_ _z) : x(_x), y(_y), z(_z) {}

			// Accessors
			T_&			operator[] (unsigned _index)		{ return (reinterpret_cast<T_*>(this))[_index]; }
			const T_&	operator[] (unsigned _index) const	{ return (reinterpret_cast<const T_*>(this))[_index]; }

			// Assignment
			Vector& operator=(const Vector&);

			// Comparison
			bool operator==(const Vector&) const;
			bool operator!=(const Vector&) const;

			// Adition and substraction
			Vector	operator+	(const Vector&) const;	// Adition
			Vector&	operator+=	(const Vector&);		// Adition
			Vector	operator-	(const Vector&) const;	// Substraction
			Vector&	operator-=	(const Vector&);		// Substraction
			Vector	operator-	() const;				// Return the oposite of this vector

			// Products
			Vector	operator*	(const T_) const;		// Product by scalar
			Vector&	operator*=	(const T_);				// Product by scalar
			Vector	operator/	(const T_) const;		// Division by scalar
			Vector&	operator/=	(const T_);				// Division by scalar
			T_		operator*	(const Vector&) const;	// Dot product
			Vector	operator^	(const Vector&) const;	// Cross product

			// Vector norm and normalization
			T_		norm() const;						//< Vector norm
			T_		sqNorm() const;						///< Vector square norm
			Vector	normalized() const;					///< Returns a normalized version of the vector
			Vector&	normalize();						///< Normalizes the vector

			// Common vectors
			static Vector zero() { return Vector(0.f, 0.f, 0.f); }
			static Vector xAxis() { return Vector(T_(1.f), T_(0.f), T_(0.f)); }
			static Vector yAxis() { return Vector(T_(0.f), T_(1.f), T_(0.f)); }
			static Vector zAxis() { return Vector(T_(0.f), T_(0.f), T_(1.f)); }

		};

		template<typename T_>
		using Vector2 = Vector<T_, 2>;
		template<typename T_>
		using Vector3 = Vector<T_, 3>;
		template<typename T_>
		using Vector4 = Vector<T_, 4>;

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

#include "vector.inl"

#endif // _REV_MATH_ALGEBRA_VECTOR_H_